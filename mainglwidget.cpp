#include "mainglwidget.h"

MainGLWidget::MainGLWidget(QWidget *parent) : QGLWidget(parent),
    xoffset(0), zoffset(1), shaderProgram(this),
    fileName(":/legoBricks/models/4x2B.obj"), isBricks(true) {

}

void MainGLWidget::initializeGL()
{
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

    // Переместим модель вглубь экрана, чтобы она была видна
    resetModelView();
}

void MainGLWidget::resizeGL(int nWidth, int nHeight)
{
    // Задание области вывода
    glViewport(0, 0, nWidth, nHeight);
    // Задаём матрицу центрального проектирования
    resetProjection();
}

// Внутри данной подпрограммы происходит рисование объектов
void MainGLWidget::paintGL()
{
    if (isBricks) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //считываем модель из fileName.obj
    readObj model(fileName);
    model.readFile();

    srand(100);
    // Массив цветов для каждой вершины
    float colors[model.points.size()][3];
    for (int i = 0; i < model.points.size(); i++) {
        colors[i][0] = (rand() % 100) / 100.0;
        colors[i][1] = (rand() % 100) / 100.0;
        colors[i][2] = (rand() % 100) / 100.0;
    }

    float vertices[model.points.size()][3];		//массив точек модели
    GLushort indeces[model.faces.size() * 3];	//массив граней, которые представляют собой индексы в массике точек
    model.getVertexAndFaces(vertices, indeces);	//получаем массив точек и граней по считанной модели

    shaderProgram.bind();

    // Зададим матрицу, на которую будут умножены однородные координаты вершин в вершинном шейдере
    shaderProgram.setUniformValue("matrix", projectMatrix * modelViewMatrix);

    // Передадим шейдеру весь массив вершин
    // Третий параметр равен трем, потому что одна вершина квадрата задана в массиве vertices тремя числами
    shaderProgram.setAttributeArray("vertex", (float*)vertices, 3);
    shaderProgram.enableAttributeArray("vertex");

    // Передаём массив цветов каждой вершины (цвет каждой вершины задаётся тремя числами)
    shaderProgram.setAttributeArray("color", (float*)colors, 3);
    shaderProgram.enableAttributeArray("color");

    // Рисование примитива по координатам, заданным в массиве
    // Второй параметр означает, что модель состоит из model.faces.size() граней, каждая из которых содержит 3 точки
    glDrawElements(GL_TRIANGLES, 3 * model.faces.size(), GL_UNSIGNED_SHORT, indeces);
    }
    else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Model model(20);

        srand(100);
        // Массив цветов для каждой вершины
        float colors[model.howPolygons() * 3][3];
        for (int i = 0; i < model.howPolygons() * 3; i++) {
        colors[i][0] = (rand() % 100) / 100.0;
        colors[i][1] = (rand() % 100) / 100.0;
        colors[i][2] = (rand() % 100) / 100.0;
        }

        float vertices[model.howPolygons() * 3][3];		//массив точек модели
        model.getStaticModel(vertices);

        shaderProgram.bind();

        // Зададим матрицу, на которую будут умножены однородные координаты вершин в вершинном шейдере
        shaderProgram.setUniformValue("matrix", projectMatrix * modelViewMatrix);

        // Передадим шейдеру весь массив вершин
        // Третий параметр равен трем, потому что одна вершина квадрата задана в массиве vertices тремя числами
        shaderProgram.setAttributeArray("vertex", (float*)vertices, 3);
        shaderProgram.enableAttributeArray("vertex");

        // Передаём массив цветов каждой вершины (цвет каждой вершины задаётся тремя числами)
        shaderProgram.setAttributeArray("color", (float*)colors, 3);
        shaderProgram.enableAttributeArray("color");

        // Рисование примитива по координатам, заданным в массиве
        // Второй параметр означает, что модель состоит из model.faces.size() граней, каждая из которых содержит 3 точки
        glDrawArrays(GL_TRIANGLES, 0, model.howPolygons() * 3);
    }

    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");
    shaderProgram.release();

    textOut();
}

void MainGLWidget::textOut(void)
{
    // Вывод на экран текста
    glColor3f(1, 1, 1);
    QFont text_font("Cambria", 10, QFont::Bold);
    renderText(10, 15, "Колесо мыши - удалить/приблизить", text_font);
    renderText(10, 30, "Зажатая кнопка мыши - повернуть сцену", text_font);
    renderText(10, 45, "Esc - выход", text_font);
}


void MainGLWidget::resetProjection()
{
    // Инициализация единичной матрицы
    projectMatrix.setToIdentity();

    // Умножение на матрицу перспективного проектирования
    projectMatrix.perspective(30.0, (float)width() / height(), 0.1, 20);
}



// Процедура для изменения видовой матрицы
void MainGLWidget::resetModelView()
{
    // Инициализация видовой матрицы как единичной
    modelViewMatrix.setToIdentity();

    // Далее аффинные преобразования записаны в обратном порядке

    // Третья операция - перенос объекта вдоль оси z (например, вглубь экрана)
    // Умножим видовую матрицу на матрицу переноса
    modelViewMatrix.translate(0, 0, -zoffset);
    modelViewMatrix.translate(xoffset, yoffset, zoffset2);

    // Вторая операция - поворот объекта
    // Умножим видовую матрицу на матрицу поворота
    modelViewMatrix *= rotateMatrix.transposed();

    // Первая операция - масштабирование объекта (уменьшим объект, чтобы он не занимал весь экран)
    modelViewMatrix.scale(0.3, 0.3, 0.3);
}


// Обработчик события перемещения указателя мыши (событие происходит при зажатой кнопке мыши)
void MainGLWidget::mouseMoveEvent(QMouseEvent* m_event)
{
    // Вычислим, на сколько переместился курсор мыши между двумя событиями mouseMoveEvent
    QPoint dp = m_event->pos() - mousePosition;
    // Изменим матрицу поворота в соответствии с тем, как пользователь переместил курсор мыши
    changeRotateMatrix(rotateMatrix, dp.x(), dp.y());
    // Сохраним текущую позицию мыши
    mousePosition = m_event->pos();
    // Обновим матрицу аффинных преобразований
    resetModelView();
    updateGL(); // Перерисовать окно
}


// Процедура предназначена для изменения матрицы поворота, чтобы куб поворачивался в нужном направлении строго вслед за указателем мыши.
// Вызывается, когда пользователь изменил положение указателя мыши при зажатой кнопке (мыши)
void MainGLWidget::changeRotateMatrix(QMatrix4x4& R, float dx, float dy)
{
    R.rotate(-dx, 0, 1, 0);         // Умножение R на матрицу поворота вокруг оси y
    R.rotate(-dy, 1, 0, 0);         // Умножение R на матрицу поворота вокруг оси x
}


// Обработчик события прокрутки колеса мыши
void MainGLWidget::wheelEvent(QWheelEvent* w_event)
{
    // При прокрутке колеса мыши изменяем глубину объекта
    zoffset -= w_event->delta() / 500.0;
    resetModelView(); // Обновим матрицу аффинных преобразований
    updateGL(); // Перерисовать окно
}


void MainGLWidget::mousePressEvent(QMouseEvent* m_event)
{
    mousePosition = m_event->pos();
}


void MainGLWidget::keyPressEvent(QKeyEvent* event)
{
    // Закрыть окно при нажатии клавиши Escape
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
}
