#include "mainglwidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent), plane(1.3, rotateMatrix),
    activeBrick(-1), deepOffset(1.3), shaderProgram(this) {

    rotateMatrix.setToIdentity();
    rotateMatrix.rotate(-10, 1, 0, 0);

    //1-ый направленный источник освещения
    lights[0].pos = QVector3D(5.0, 3.0, 0.0);
    lights[0].ambient = QVector3D(0.0, 0.0, 0.0);
    lights[0].diffuse = QVector3D(0.98, 0.92, 0.84);
    lights[0].isOn = true;

    //2-ой направленный источник освещения
    lights[1].pos = QVector3D(0.0, -5.0, 0.0);
    lights[1].ambient = QVector3D(0.0, 0.0, 0.0);
    lights[1].diffuse =  QVector3D(0.61, 0.94, 0.91);
    lights[1].isOn = true;

    //3-ий точечный источник освещения
    lights[2].pos = QVector3D(0.0, 0.0, 0.0);
    lights[2].ambient = QVector3D(0.0, 0.0, 0.0);
    lights[2].diffuse = QVector3D(1.0, 1.0, 0.0);
    lights[2].isOn = true;

    plane.setRotateMatrix(rotateMatrix);
    plane.resetModelView();
}

void MainGLWidget::initializeGL() {

    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);
    // Режим рисования только лицевых граней

    // Фоновый цвет
    qglClearColor(QColor(Qt::black));

    // Инициализация шейдеров

    // Вершинный шейдер
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
            bricks[i]->draw(shaderProgram, true, lights);
        else
            bricks[i]->draw(shaderProgram, false, lights);
    }

    plane.draw(shaderProgram, false, lights);
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

// Процедура предназначена для изменения матрицы поворота, чтобы объекты поворачивалиcь в нужном направлении строго вслед за указателем мыши.
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
        brick->setDeepOffset(deepOffset);
    plane.setDeepOffset(deepOffset);
    updateGL(); // Перерисовать окно
}


void MainGLWidget::mousePressEvent(QMouseEvent* m_event) {

    mousePosition = m_event->pos();
    getWorldPos(m_event->pos());
    setActiveBrick();
    updateGL();
}


QVector3D MainGLWidget::myUnProject(QVector3D point) {

    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.setToIdentity();
    QVector4D viewport(0, 0, width(), height());

    QVector4D transformPoint;
    transformPoint.setX((point.x() - viewport.x()) / (float)viewport.z() * 2.0 - 1.0);
    transformPoint.setY((point.y() - viewport.y()) / (float)viewport.w() * 2.0 - 1.0);
    transformPoint.setZ(2.0 * point.z() - 1.0);
    transformPoint.setW(1.0);

    QVector4D resultPoint = transformPoint * modelViewMatrix;

    resultPoint.setW(1.0 / resultPoint.w());
    resultPoint.setX(resultPoint.x() * resultPoint.w());
    resultPoint.setY(resultPoint.y() * resultPoint.w());
    resultPoint.setZ(resultPoint.z() * resultPoint.w());

    return QVector3D(resultPoint.x(), resultPoint.y(), resultPoint.z());
}


void MainGLWidget::getWorldPos(QPoint screenPos) {

    float x = screenPos.x();
    float y = height() - screenPos.y()- 1;

    QVector3D start(x, y, -1.0f);
    QVector3D end(x, y, 1.0f);

    sRayBegin = myUnProject(start);
    sRayEnd = myUnProject(end);
}


void MainGLWidget::setActiveBrick() {

    int i = 0;
    float minDepth = (float)INT_MAX;
    int currentActiveBrick = -1;
    float currentDepth;

    // для каждого объекта на сцене
    for (auto brick : bricks) {

        //определяем пересекается ли он селект. лучом
        if (brick->pointInBrick(sRayBegin, sRayEnd, currentDepth)) {

            //если да, то сраваниваем его глубину с текущей
            if (currentDepth < minDepth) {
                minDepth = currentDepth;
                currentActiveBrick = i;
            }
        }

        i++;
    }

    //в итоге находим ближайшим к нас объект и "выбираем" его, или оставляем прежним, если селект. луч не пересекается ни с одним объектом
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

void MainGLWidget::setLight1(bool flag) {
    lights[0].isOn = !flag;
    updateGL();
}

void MainGLWidget::setLight2(bool flag) {
    lights[1].isOn = !flag;
    updateGL();
}

void MainGLWidget::setLight3(bool flag) {
    lights[2].isOn = !flag;
    updateGL();
}

MainGLWidget::~MainGLWidget() {

    for (int i = 0; i < bricks.size(); i++)
        delete bricks[i];
}


