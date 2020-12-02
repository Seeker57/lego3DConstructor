#include "mainglwidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent), deepOffset(1.3), shaderProgram(this) {

}

void MainGLWidget::initializeGL() {

    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);

    // Фоновый цвет
    qglClearColor(QColor(Qt::black));

    rotateMatrix.setToIdentity();

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
}

// Внутри данной подпрограммы происходит рисование объектов
void MainGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < bricks.size(); i++)
        bricks[i]->draw(shaderProgram);

    textOut();
}

void MainGLWidget::textOut(void) {

    // Вывод на экран текста
    glColor3f(1, 1, 1);
    QFont text_font("Cambria", 10, QFont::Bold);
    renderText(10, 15, "Колесо мыши - удалить/приблизить", text_font);
    renderText(10, 30, "Зажатая кнопка мыши - повернуть сцену", text_font);
    renderText(10, 45, "Esc - выход", text_font);
}

// Обработчик события перемещения указателя мыши (событие происходит при зажатой кнопке мыши)
void MainGLWidget::mouseMoveEvent(QMouseEvent* m_event) {

    // Вычислим, на сколько переместился курсор мыши между двумя событиями mouseMoveEvent
    QPoint dp = m_event->pos() - mousePosition;
    changeRotateMatrix(dp.x(), dp.y());

    // Изменим матрицу поворота в соответствии с тем, как пользователь переместил курсор мыши
    for (auto brick : bricks) {
        brick->setRotateMatrix(rotateMatrix);
        brick->resetModelView();
    }

    // Сохраним текущую позицию мыши
    mousePosition = m_event->pos();
    updateGL(); // Перерисовать окно
}

// Процедура предназначена для изменения матрицы поворота, чтобы куб поворачивался в нужном направлении строго вслед за указателем мыши.
// Вызывается, когда пользователь изменил положение указателя мыши при зажатой кнопке (мыши)
void MainGLWidget::changeRotateMatrix(float dx, float dy) {

    rotateMatrix.rotate(-dx, 0, 1, 0);         // Умножение R на матрицу поворота вокруг оси y
    rotateMatrix.rotate(-dy, 1, 0, 0);         // Умножение R на матрицу поворота вокруг оси x
}

// Обработчик события прокрутки колеса мыши
void MainGLWidget::wheelEvent(QWheelEvent* w_event) {

    // При прокрутке колеса мыши изменяем глубину объекта
    deepOffset -= w_event->delta() / 500.0;

    for (auto brick : bricks)
        brick->setDeepOffset(deepOffset); // Обновим матрицу аффинных преобразований
    updateGL(); // Перерисовать окно
}


void MainGLWidget::mousePressEvent(QMouseEvent* m_event) {

    mousePosition = m_event->pos();
}


void MainGLWidget::keyPressEvent(QKeyEvent* event) {

    // Закрыть окно при нажатии клавиши Escape
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void MainGLWidget::addBrick(QString fileName) {

    Brick* newBrick = new Brick(fileName, deepOffset, rotateMatrix);
    bricks.push_back(newBrick); updateGL();
    bricks[bricks.size() - 1]->getWindowSize(width(), height());
    bricks[bricks.size() - 1]->resetProjection();
    updateGL();
}

void MainGLWidget::addMyModel() {

    Brick* newBrick = new MyModel(deepOffset, rotateMatrix);
    bricks.push_back(newBrick);
    bricks[bricks.size() - 1]->getWindowSize(width(), height());
    bricks[bricks.size() - 1]->resetProjection();
    updateGL();
}

MainGLWidget::~MainGLWidget() {

    for (int i = 0; i < bricks.size(); i++)
        delete bricks[i];
}
