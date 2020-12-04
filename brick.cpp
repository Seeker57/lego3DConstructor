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

    // Вторая операция - поворот объекта, а так же перенос по заданным координатам
    // Умножим видовую матрицу на матрицу поворота
    modelViewMatrix *= rotateMatrix.transposed();
    modelViewMatrix.translate(xoffset, zoffset, yoffset);
    modelViewMatrix *= rotatePushMatrix.transposed();

    // Первая операция - масштабирование объекта (уменьшим объект, чтобы он не занимал весь экран)
    modelViewMatrix.scale(0.3, 0.3, 0.3);
}

bool Brick::pointInBrick(QVector3D sRayBegin, QVector3D sRayEnd, float& minDepth) {

    bool inBrick = false;
    float currentDepth = (float)INT_MAX;

    for (int i = 0; i < model.faces.size(); i++) {

        QVector3D currentFaces[3];
        currentFaces[0] = projectMatrix * modelViewMatrix * model.points[model.faces[i][0]];
        currentFaces[1] = projectMatrix * modelViewMatrix * model.points[model.faces[i][1]];
        currentFaces[2] = projectMatrix * modelViewMatrix * model.points[model.faces[i][2]];

        if (isCrossWithSL(currentFaces, sRayBegin, sRayEnd)) {

            QVector3D depth = (currentFaces[0] + currentFaces[1] + currentFaces[2]) / 3.0;
            if (depth.z() < currentDepth) {
                currentDepth = depth.z();
                inBrick = true;
            }
        }
    }

    minDepth = currentDepth;
    return inBrick;
}

int Brick::sgn(const float& k) {

  if( k > 0 )
      return 1;
  if( k < 0 )
      return -1;
  return 0;
}

bool Brick::isCrossWithSL(QVector3D currentFaces[], QVector3D sRayBegin, QVector3D sRayEnd) {

    QVector3D normal = QVector3D::normal(currentFaces[0], currentFaces[1], currentFaces[2]);

    float r1 = QVector3D::dotProduct(normal, sRayBegin - currentFaces[0]);
    float r2 = QVector3D::dotProduct(normal, sRayEnd - currentFaces[0]);

    if (sgn(r1) == sgn(r2))
        return false;

    QVector3D ip = (sRayBegin + ((sRayEnd - sRayBegin) * (-r1 / (r2 - r1))));

    if (QVector3D::dotProduct(QVector3D::crossProduct(currentFaces[1] - currentFaces[0], ip - currentFaces[0]), normal) <= 0)
        return false;
    if (QVector3D::dotProduct(QVector3D::crossProduct(currentFaces[2] - currentFaces[1], ip - currentFaces[1]), normal) <= 0)
        return false;
    if (QVector3D::dotProduct(QVector3D::crossProduct(currentFaces[0] - currentFaces[2], ip - currentFaces[2]), normal) <= 0)
        return false;

    return true;
}

void Brick::draw(QGLShaderProgram &shaderProgram, bool isActive) {

    srand(100);

    // Массив цветов для каждой вершины
    float colors[model.points.size()][3];

    float colorRange;
    if (isActive)
        colorRange = 100;
    else
        colorRange = 200;

    for (int i = 0; i < model.points.size(); i++) {

        colors[i][0] = (rand() % 100) / colorRange;
        colors[i][1] = (rand() % 100) / colorRange;
        colors[i][2] = (rand() % 100) / colorRange;
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

void MyModel::draw(QGLShaderProgram &shaderProgram, bool isActive) {

    Model myModel(20);

    srand(100);
    // Массив цветов для каждой вершины
    float colors[myModel.howPolygons() * 3][3];

    float colorRange;
    if (isActive)
        colorRange = 100;
    else
        colorRange = 200;

    for (int i = 0; i < myModel.howPolygons() * 3; i++) {

        colors[i][0] = (rand() % 100) / colorRange;
        colors[i][1] = (rand() % 100) / colorRange;
        colors[i][2] = (rand() % 100) / colorRange;
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

bool MyModel::pointInBrick(QVector3D sRayBegin, QVector3D sRayEnd, float& minDepth) {

    Model myModel(20);
    bool inBrick = false;
    float currentDepth = (float)INT_MAX;

    for (int i = 0; i < myModel.howPolygons(); i++) {

        QVector3D currentFaces[3];
        currentFaces[0] = projectMatrix * modelViewMatrix * myModel[i][0].point;
        currentFaces[1] = projectMatrix * modelViewMatrix * myModel[i][1].point;
        currentFaces[2] = projectMatrix * modelViewMatrix * myModel[i][2].point;

        if (isCrossWithSL(currentFaces, sRayBegin, sRayEnd)) {

            QVector3D depth = (currentFaces[0] + currentFaces[1] + currentFaces[2]) / 3.0;
            if (depth.z() < currentDepth) {
                currentDepth = depth.z();
                inBrick = true;
            }
        }
    }

    minDepth = currentDepth;
    return inBrick;
}
