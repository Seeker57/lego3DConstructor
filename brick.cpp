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

    //для каждой грани объекта
    for (int i = 0; i < model.faces.size(); i++) {

        QVector3D currentFaces[3];
        currentFaces[0] = projectMatrix * modelViewMatrix * model.points[model.faces[i][0]];
        currentFaces[1] = projectMatrix * modelViewMatrix * model.points[model.faces[i][1]];
        currentFaces[2] = projectMatrix * modelViewMatrix * model.points[model.faces[i][2]];

        //проверяем пересекается ли она селект. лучом
        if (isCrossWithSL(currentFaces, sRayBegin, sRayEnd)) {

            //если да, то считаем глубину этой грани с сравниваем с текущей
            QVector3D depth = (currentFaces[0] + currentFaces[1] + currentFaces[2]) / 3.0;
            if (depth.z() < currentDepth) {
                currentDepth = depth.z();
                inBrick = true;
            }
        }
    }

    //в итоге минимальную глубину среди граней, которые пересекаются селект. лучом
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

//определяет, пересекается ли  треугольная грань в пространстве селект. лучом, который задан точками sRayBegin и sRayEnd
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

    //выбираем диапазон цветов, если объект выбран, то более яркие, если нет, то более темные
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
    getModels(vertices, indeces);				//получаем массив точек и граней по считанной модели

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

PlaneAndAxis::PlaneAndAxis(float deepOf, QMatrix4x4 R) : Brick() {

    xoffset = yoffset = zoffset = 0.0;
    deepOffset = deepOf;
    rotateMatrix = R;
    resetModelView();
}

void PlaneAndAxis::draw(QGLShaderProgram &shaderProgram, bool isActive) {

    float vertices[44 * 2][3];		//массив точек модели
    GLushort indeces[44 * 2];		//массив граней, которые представляют собой индексы в массике точек

    //начинаем построение сетки с самых дальних точек
    QVector3D leftPY(21 * VALUE_TURN_X, 21 * VALUE_TURN_Y, 0.0);
    QVector3D rightPY(-21 * VALUE_TURN_X, 21 * VALUE_TURN_Y, 0.0);
    QVector3D leftPX(21 * VALUE_TURN_X, 21 * VALUE_TURN_Y, 0.0);
    QVector3D rightPX(21 * VALUE_TURN_X, -21 * VALUE_TURN_Y, 0.0);

    //после чего по каждой координате x и y отступаем соотв. расстояние, кратное размеру Блока по соотв. координате
    for (int i = 0; i < 44 * 2; i += 4) {

        vertices[i][0] = leftPY.x();
        vertices[i][1] = leftPY.y();
        vertices[i][2] = leftPY.z();
        vertices[i + 1][0] = rightPY.x();
        vertices[i + 1][1] = rightPY.y();
        vertices[i + 1][2] = rightPY.z();
        leftPY.setY(leftPY.y() - 2 * VALUE_TURN_Y);
        rightPY.setY(rightPY.y() - 2 * VALUE_TURN_Y);

        vertices[i + 2][0] = leftPX.x();
        vertices[i + 2][1] = leftPX.y();
        vertices[i + 2][2] = leftPX.z();
        vertices[i + 3][0] = rightPX.x();
        vertices[i + 3][1] = rightPX.y();
        vertices[i + 3][2] = rightPX.z();
        leftPX.setX(leftPX.x() - 2 * VALUE_TURN_X);
        rightPX.setX(rightPX.x() - 2 * VALUE_TURN_X);
    }

    //иниц. массив вершин линий
    for (int i = 0; i < 44 * 2; i += 4) {

        indeces[i] = i;
        indeces[i + 1] = i + 1;
        indeces[i + 2] = i + 2;
        indeces[i + 3] = i + 3;
    }

    // Массив цветов для каждой вершины
    float colors[44 * 2][3];

    for (int i = 0; i < 44 * 2; i++) {

        colors[i][0] = 0.3f;
        colors[i][1] = 0.3f;
        colors[i][2] = 0.3f;
    }

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
    glDrawElements(GL_LINES, 2 * 44, GL_UNSIGNED_SHORT, indeces);

    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");

    //координаты точек линий для координатных осей, а так же их цвета
    //x - красная линия, y - зеленая линия, z - синяя
    float lineXVert[2][3] = {{-2, 0, 0}, {2, 0, 0}};
    float lineXColor[2][3] = {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}};
    float lineYVert[2][3] = {{0, 2, 0}, {0, -2, 0}};
    float lineYColor[2][3] = {{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}};
    float lineZVert[2][3] = {{0, 0, 0}, {0, 0, -2}};
    float lineZColor[2][3] = {{0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}};

    shaderProgram.setAttributeArray("vertex", (float*)lineXVert, 3);
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.setAttributeArray("color", (float*)lineXColor, 3);
    shaderProgram.enableAttributeArray("color");
    glDrawArrays(GL_LINES, 0, 2);
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");

    shaderProgram.setAttributeArray("vertex", (float*)lineYVert, 3);
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.setAttributeArray("color", (float*)lineYColor, 3);
    shaderProgram.enableAttributeArray("color");
    glDrawArrays(GL_LINES, 0, 2);
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");

    shaderProgram.setAttributeArray("vertex", (float*)lineZVert, 3);
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.setAttributeArray("color", (float*)lineZColor, 3);
    shaderProgram.enableAttributeArray("color");
    glDrawArrays(GL_LINES, 0, 2);
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("color");

    shaderProgram.release();
}

void PlaneAndAxis::resetModelView() {
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
    modelViewMatrix.rotate(90, 1, 0, 0);
}

