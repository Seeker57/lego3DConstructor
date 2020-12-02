#include "brick.h"

Brick::Brick(QString fileName, float deepOf, QMatrix4x4 R) : model(fileName), xoffset(0.0), yoffset(0.0), zoffset(0.0),
    deepOffset(deepOf), rotateMatrix(R) {

    model.readFile();
    rotatePushMatrix.setToIdentity();
    resetModelView();
}

void Brick::resetProjection()
{
    // Инициализация единичной матрицы
    projectMatrix.setToIdentity();

    // Умножение на матрицу перспективного проектирования
    projectMatrix.perspective(30.0, (float)width / height, 0.1, 20);
}



// Процедура для изменения видовой матрицы
void Brick::resetModelView()
{
    // Инициализация видовой матрицы как единичной
    modelViewMatrix.setToIdentity();

    // Далее аффинные преобразования записаны в обратном порядке

    // Третья операция - перенос объекта вдоль оси z (например, вглубь экрана)
    // Умножим видовую матрицу на матрицу переноса
    modelViewMatrix.translate(0, 0, -deepOffset);

    // Вторая операция - поворот объекта
    // Умножим видовую матрицу на матрицу поворота
    modelViewMatrix *= rotateMatrix.transposed();
    modelViewMatrix.translate(xoffset, zoffset, yoffset);
    modelViewMatrix *= rotatePushMatrix.transposed();

    // Первая операция - масштабирование объекта (уменьшим объект, чтобы он не занимал весь экран)
    modelViewMatrix.scale(0.3, 0.3, 0.3);
}

void Brick::draw(QGLShaderProgram &shaderProgram) {

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
    getModels(vertices, indeces);	//получаем массив точек и граней по считанной модели

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

    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");
    shaderProgram.release();
}

MyModel::MyModel(float deepOf, QMatrix4x4 R) : Brick() {

    xoffset = yoffset = zoffset = 0.0;
    deepOffset = deepOf;
    rotateMatrix = R;
    resetModelView();
}

void MyModel::draw(QGLShaderProgram &shaderProgram) {

    Model myModel(20);

    srand(100);
    // Массив цветов для каждой вершины
    float colors[myModel.howPolygons() * 3][3];
    for (int i = 0; i < myModel.howPolygons() * 3; i++) {

        colors[i][0] = (rand() % 100) / 100.0;
        colors[i][1] = (rand() % 100) / 100.0;
        colors[i][2] = (rand() % 100) / 100.0;
    }

    float vertices[myModel.howPolygons() * 3][3];		//массив точек модели
    myModel.getStaticModel(vertices);

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
    glDrawArrays(GL_TRIANGLES, 0, myModel.howPolygons() * 3);

    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");
    shaderProgram.release();
}
