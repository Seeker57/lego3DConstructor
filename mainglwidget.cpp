#include "mainglwidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent), shaderProgram(this) {

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

    // Изменим матрицу поворота в соответствии с тем, как пользователь переместил курсор мыши
    for (auto brick : bricks) {
        brick->changeRotateMatrix(dp.x(), dp.y());
        brick->resetModelView();
    }

    // Сохраним текущую позицию мыши
    mousePosition = m_event->pos();
    updateGL(); // Перерисовать окно
}



// Обработчик события прокрутки колеса мыши
void MainGLWidget::wheelEvent(QWheelEvent* w_event) {

    // При прокрутке колеса мыши изменяем глубину объекта
    float deepOffset = w_event->delta() / 500.0;

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

    Brick* newBrick = new Brick(fileName);
    bricks.push_back(newBrick); updateGL();
    bricks[bricks.size() - 1]->getWindowSize(width(), height());
    bricks[bricks.size() - 1]->resetProjection();
    updateGL();
}

void MainGLWidget::addMyModel() {

    Brick* newBrick = new MyModel();
    bricks.push_back(newBrick);
    bricks[bricks.size() - 1]->getWindowSize(width(), height());
    bricks[bricks.size() - 1]->resetProjection();
    updateGL();
}

MainGLWidget::~MainGLWidget() {

    for (int i = 0; i < bricks.size(); i++)
        delete bricks[i];
}
