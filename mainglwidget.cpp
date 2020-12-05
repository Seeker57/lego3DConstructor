#include "mainglwidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent), plane(1.3, rotateMatrix),
    activeBrick(-1), deepOffset(1.3), shaderProgram(this) {

    rotateMatrix.setToIdentity();
    rotateMatrix.rotate(-10, 1, 0, 0);
    plane.setRotateMatrix(rotateMatrix);
    plane.resetModelView();
}

void MainGLWidget::initializeGL() {

    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);

    // Фоновый цвет
    qglClearColor(QColor(Qt::black));

    // Инициализация шейдеров

    // Вершинный шейдер
    // Шейдер принимает на вход вектор из трех координат (x, y, z)
    // При расчёте выходного вектора зададим координату w = 1
    shaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/GLSL/vertexShader.vsh");

    // Фрагментный шейдер задаёт только цвет
    shaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/GLSL/fragmentShader.fsh");

    // Сборка шейдеров
    if(shaderProgram.link() == false)
      qDebug() << shaderProgram.log();
}

void MainGLWidget::resizeGL(int nWidth, int nHeight) {

    // Задание области вывода
    glViewport(0, 0, nWidth, nHeight);

    // Задаём матрицу центрального проектирования
    for (int i = 0; i < bricks.size(); i++) {
        bricks[i]->getWindowSize(width(), height());
        bricks[i]->resetProjection();
    }

    plane.getWindowSize(width(), height());
    plane.resetProjection();
}

// Внутри данной подпрограммы происходит рисование объектов
void MainGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < bricks.size(); i++) {

        if (activeBrick == i)
            bricks[i]->draw(shaderProgram, true);
        else
            bricks[i]->draw(shaderProgram, false);
    }

    plane.draw(shaderProgram, false);
    textOut();
}

void MainGLWidget::textOut(void) {

    // Вывод на экран текста
    glColor3f(1, 1, 1);
    QFont text_font("Cambria", 10, QFont::Bold);
    renderText(10, 15, "Кнопки сверху - для добавления/удаления деталей", text_font);
    renderText(10, 30, "Выбирать детали - с помощью клавиш мыши. Созданная деталь автом. выбирается", text_font);
    renderText(10, 45, "Изменять положение детали в пространстве - с помощью кнопок ниже или клавиш:", text_font);
    renderText(10, 60, "по оси x - A/D, по оси y - Q/E, по оси z - W/S", text_font);
    renderText(10, 75, "Колесо мыши - удалить/приблизить", text_font);
    renderText(10, 90, "Зажатая кнопка мыши - повернуть сцену", text_font);
    renderText(10, 105, "Esc - выход", text_font);
}

// Обработчик события перемещения указателя мыши (событие происходит при зажатой кнопке мыши)
void MainGLWidget::mouseMoveEvent(QMouseEvent* m_event) {

    // Вычислим, на сколько переместился курсор мыши между двумя событиями mouseMoveEvent
    QPointF dp = m_event->pos() - mousePosition;
    changeRotateMatrix(dp.x(), dp.y());

    // Изменим матрицу поворота в соответствии с тем, как пользователь переместил курсор мыши
    for (auto brick : bricks) {
        brick->setRotateMatrix(rotateMatrix);
        brick->resetModelView();
    }
    plane.setRotateMatrix(rotateMatrix);
    plane.resetModelView();

    // Сохраним текущую позицию мыши
    mousePosition = m_event->pos();
    updateGL(); // Перерисовать окно
}

// Процедура предназначена для изменения матрицы поворота, чтобы куб поворачивался в нужном направлении строго вслед за указателем мыши.
// Вызывается, когда пользователь изменил положение указателя мыши при зажатой кнопке (мыши)
void MainGLWidget::changeRotateMatrix(float dx, float dy) {

    rotateMatrix.rotate(-dx / CAMERA_SPEED, 0, 1, 0);         // Умножение R на матрицу поворота вокруг оси y
    rotateMatrix.rotate(-dy / CAMERA_SPEED, 1, 0, 0);         // Умножение R на матрицу поворота вокруг оси x
}

// Обработчик события прокрутки колеса мыши
void MainGLWidget::wheelEvent(QWheelEvent* w_event) {

    // При прокрутке колеса мыши изменяем глубину объекта
    deepOffset -= w_event->delta() / 500.0;

    for (auto brick : bricks)
        brick->setDeepOffset(deepOffset); // Обновим матрицу аффинных преобразований
    plane.setDeepOffset(deepOffset);
    updateGL(); // Перерисовать окно
}


void MainGLWidget::mousePressEvent(QMouseEvent* m_event) {

    mousePosition = m_event->pos();
    getWorldPos(m_event->pos());
    setActiveBrick();
    updateGL();
}


void MainGLWidget::getWorldPos(QPoint screenPos) {

    GLint viewport[4];
    GLdouble mvm[16];
    GLdouble projm[16];
    GLdouble wx, wy, wz;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvm);
    glGetDoublev(GL_PROJECTION_MATRIX, projm);

    float x = screenPos.x();
    float y = height() - screenPos.y() - 1;

    gluUnProject(x, y, -1.0f, mvm, projm, viewport, &wx, &wy, &wz);
    sRayBegin = QVector3D(wx, wy, wz);

    gluUnProject(x, y, 1.0f, mvm, projm, viewport, &wx, &wy, &wz);
    sRayEnd = QVector3D(wx, wy, wz);
}


void MainGLWidget::setActiveBrick() {

    int i = 0;
    float minDepth = (float)INT_MAX;
    int currentActiveBrick = -1;
    float currentDepth;

    for (auto brick : bricks) {
        if (brick->pointInBrick(sRayBegin, sRayEnd, currentDepth)) {

            if (currentDepth < minDepth) {
                minDepth = currentDepth;
                currentActiveBrick = i;
            }
        }

        i++;
    }

    if (currentActiveBrick != -1)
        activeBrick = currentActiveBrick;
}

void MainGLWidget::addBrick(QString fileName) {

    Brick* newBrick = new Brick(fileName, deepOffset, rotateMatrix);
    bricks.push_back(newBrick); updateGL();
    bricks[bricks.size() - 1]->getWindowSize(width(), height());
    bricks[bricks.size() - 1]->resetProjection();
    activeBrick = bricks.size() - 1;
    updateGL();
}

void MainGLWidget::addMyModel() {

    Brick* newBrick = new MyModel(deepOffset, rotateMatrix);
    bricks.push_back(newBrick);
    bricks[bricks.size() - 1]->getWindowSize(width(), height());
    bricks[bricks.size() - 1]->resetProjection();
    activeBrick = bricks.size() - 1;
    updateGL();
}

MainGLWidget::~MainGLWidget() {

    for (int i = 0; i < bricks.size(); i++)
        delete bricks[i];
}

