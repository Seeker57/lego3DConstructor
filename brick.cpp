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

void Brick::setLighting(QGLShaderProgram& shaderProgram, LightInfo lights[]) {

    for (int i = 0; i < 3; i++)
        lights[i].object = QVector3D(lights[i].object.x() / (255.0 * 4),
                                     lights[i].object.y() / (255.0 * 4),
                                     lights[i].object.z() / (255.0 * 4));

    QVector3D curPos;

    if (lights[0].isOn) {
        curPos = lights[0].pos;
        curPos = rotateMatrix * curPos;
        curPos.setZ(-deepOffset);
        shaderProgram.setUniformValue("lights[0].ambientColor", lights[0].ambient);
        shaderProgram.setUniformValue("lights[0].diffuseColor", lights[0].diffuse);
        shaderProgram.setUniformValue("lights[0].objectColor", lights[0].object);
        shaderProgram.setUniformValue("lights[0].pos", curPos);
    }
    else {
        curPos = lights[0].pos;
        curPos = rotateMatrix * curPos;
        curPos.setZ(-deepOffset);
        shaderProgram.setUniformValue("lights[0].ambientColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[0].diffuseColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[0].objectColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[0].pos", curPos);
    }

    if (lights[1].isOn) {
        curPos = lights[1].pos;
        curPos = rotateMatrix * curPos;
        curPos.setZ(-deepOffset);
        shaderProgram.setUniformValue("lights[1].ambientColor", lights[1].ambient);
        shaderProgram.setUniformValue("lights[1].diffuseColor", lights[1].diffuse);
        shaderProgram.setUniformValue("lights[1].objectColor", lights[1].object);
        shaderProgram.setUniformValue("lights[1].pos", curPos);
    }
    else {
        curPos = lights[1].pos;
        curPos = rotateMatrix * curPos;
        curPos.setZ(-deepOffset);
        shaderProgram.setUniformValue("lights[1].ambientColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[1].diffuseColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[1].objectColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[1].pos", curPos);
    }

    if (lights[2].isOn) {
        curPos = lights[2].pos;
        curPos = rotateMatrix * curPos;
        curPos.setZ(-deepOffset);
        shaderProgram.setUniformValue("lights[2].ambientColor", lights[2].ambient);
        shaderProgram.setUniformValue("lights[2].diffuseColor", lights[2].diffuse);
        shaderProgram.setUniformValue("lights[2].objectColor", lights[2].object);
        shaderProgram.setUniformValue("lights[2].pos", curPos);
    }
    else {
        curPos = lights[1].pos;
        curPos = rotateMatrix * curPos;
        curPos.setZ(-deepOffset);
        shaderProgram.setUniformValue("lights[2].ambientColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[2].diffuseColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[2].objectColor", QVector3D(0.0, 0.0, 0.0));
        shaderProgram.setUniformValue("lights[2].pos", curPos);
    }
}

void Brick::draw(QGLShaderProgram &shaderProgram, bool isActive, LightInfo lights[], GLuint tid) {

    srand(100);

    //выбираем диапазон цветов, если объект выбран, то более яркие, если нет, то более темные
    float colorRange;
    if (isActive)
        colorRange = 1;
    else
        colorRange = 1.5;

    QColor brickColor = QColor(214 / colorRange, 255 / colorRange, 250 / colorRange);
    for (int i = 0; i < 3; i++)
        lights[i].object = QVector3D(brickColor.red(), brickColor.green(), brickColor.blue());

    float vertices[model.faces.size() * 3][3];		//массив точек модели
    float normals[model.faces.size() * 3][3];		//массив нормалей к вершинам
    float texturePos[model.faces.size() * 3][2];	//массив текстурных координат
    getModels(vertices, normals, texturePos);		//получаем массив точек, нормалей и граней по считанной модели

    shaderProgram.bind();

    glBindTexture(GL_TEXTURE_2D, tid);
    setLighting(shaderProgram, lights);

    // Зададим матрицы для вершинного шейдера
    shaderProgram.setUniformValue("matrix", projectMatrix * modelViewMatrix);		//матрица на которую будут умножаться координаты
    shaderProgram.setUniformValue("normalMatrix", modelViewMatrix.normalMatrix());	//матрица для пересчёта векторов нормалей
    shaderProgram.setUniformValue("modelViewMatrix", modelViewMatrix);				//видовая матрица для расчёта освещения
    shaderProgram.setUniformValue("color", brickColor);

    // Передадим шейдеру весь массив вершин
    // Третий параметр равен трем, потому что одна вершина квадрата задана в массиве vertices тремя числами
    shaderProgram.setAttributeArray("vertex", (float*)vertices, 3);
    shaderProgram.enableAttributeArray("vertex");

    // Передаем шейдеру весь массив нормалей
    shaderProgram.setAttributeArray("normal", (float*)normals, 3);
    shaderProgram.enableAttributeArray("normal");

    // Передаем шейдеру весь массив текстурных координат
    shaderProgram.setAttributeArray("texture", (float*)texturePos, 2);
    shaderProgram.enableAttributeArray("texture");

    // Рисование примитива по координатам, заданным в массиве
    // Второй параметр означает, что модель состоит из model.faces.size() граней, каждая из которых содержит 3 точки
    glDrawArrays(GL_TRIANGLES, 0, model.faces.size() * 3);

    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("texture");
    shaderProgram.release();
}

MyModel::MyModel(float deepOf, QMatrix4x4 R) : Brick() {

    xoffset = yoffset = zoffset = 0.0;
    deepOffset = deepOf;
    rotateMatrix = R;
    resetModelView();
}

void MyModel::draw(QGLShaderProgram &shaderProgram, bool isActive, LightInfo lights[], GLuint tid) {

    Model myModel(20);

    srand(100);

    //выбираем диапазон цветов, если объект выбран, то более яркие, если нет, то более темные
    float colorRange;
    if (isActive)
        colorRange = 1;
    else
        colorRange = 1.5;

    QColor brickColor = QColor(214 / colorRange, 255 / colorRange, 250 / colorRange);
    for (int i = 0; i < 3; i++)
        lights[i].object = QVector3D(brickColor.red(), brickColor.green(), brickColor.blue());

    float vertices[myModel.howPolygons() * 3][3];	//массив точек модели
    float normals[myModel.howPolygons() * 3][3];	//массив точек модели
    float texturePos[myModel.howPolygons() * 3][2];	//массив текстурных координат

    myModel.getStaticModel(vertices, normals, texturePos);

    shaderProgram.bind();

    glBindTexture(GL_TEXTURE_2D, tid);
    setLighting(shaderProgram, lights);

    // Зададим матрицы для вершинного шейдера
    shaderProgram.setUniformValue("matrix", projectMatrix * modelViewMatrix);		//матрица на которую будут умножаться координаты
    shaderProgram.setUniformValue("normalMatrix", modelViewMatrix.normalMatrix());	//матрица для пересчёта векторов нормалей
    shaderProgram.setUniformValue("modelViewMatrix", modelViewMatrix);				//видовая матрица для расчёта освещения
    shaderProgram.setUniformValue("color", brickColor);

    // Передадим шейдеру весь массив вершин
    // Третий параметр равен трем, потому что одна вершина квадрата задана в массиве vertices тремя числами
    shaderProgram.setAttributeArray("vertex", (float*)vertices, 3);
    shaderProgram.enableAttributeArray("vertex");

    // Передаем шейдеру весь массив нормалей
    shaderProgram.setAttributeArray("normal", (float*)normals, 3);
    shaderProgram.enableAttributeArray("normal");

    // Передаем шейдеру весь массив текстурных координат
    shaderProgram.setAttributeArray("texture", (float*)texturePos, 2);
    shaderProgram.enableAttributeArray("texture");

    // Рисование примитива по координатам, заданным в массиве
    // Второй параметр означает, что модель состоит из model.faces.size() граней, каждая из которых содержит 3 точки
    glDrawArrays(GL_TRIANGLES, 0, myModel.howPolygons() * 3);

    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("normal");
    shaderProgram.disableAttributeArray("texture");
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

void PlaneAndAxis::draw(QGLShaderProgram &shaderProgram, bool isActive, LightInfo lights[], GLuint tid) {

    float value_turn_x = 0.0832;
    float value_turn_y = 0.0832;

    shaderProgram.bind();
    glBindTexture(GL_TEXTURE_2D, tid);

    // Зададим матрицу, на которую будут умножены однородные координаты вершин в вершинном шейдере
    shaderProgram.setUniformValue("matrix", projectMatrix * modelViewMatrix);

    //координаты точек линий для координатных осей, а так же их цвета
    //x - красная линия, y - зеленая линия, z - синяя
    float lineXVert[2][3] = {{-2, 0, -0.001}, {2, 0, -0.001}};
    float lineYVert[2][3] = {{0, 2, -0.001}, {0, -2, -0.001}};
    float lineZVert[2][3] = {{0, 0, -0.001}, {0, 0, -2}};

    glLineWidth(2);
    shaderProgram.setUniformValue("color", QColor(150, 0, 0));
    shaderProgram.setAttributeArray("vertex", (float*)lineXVert, 3);
    shaderProgram.enableAttributeArray("vertex");
    glDrawArrays(GL_LINES, 0, 2);
    shaderProgram.disableAttributeArray("vertex");

    shaderProgram.setUniformValue("color", QColor(0, 255, 0));
    shaderProgram.setAttributeArray("vertex", (float*)lineYVert, 3);
    shaderProgram.enableAttributeArray("vertex");
    glDrawArrays(GL_LINES, 0, 2);
    shaderProgram.disableAttributeArray("vertex");

    shaderProgram.setUniformValue("color", QColor(0, 0, 255));
    shaderProgram.setAttributeArray("vertex", (float*)lineZVert, 3);
    shaderProgram.enableAttributeArray("vertex");
    glDrawArrays(GL_LINES, 0, 2);
    shaderProgram.disableAttributeArray("vertex");

    float plane[4][3] = { {21 * value_turn_x, 21 * value_turn_y, 0.0}, {21 * value_turn_x, -21 * value_turn_y, 0.0},
                          {-21 * value_turn_x, -21 * value_turn_y, 0.0}, {-21 * value_turn_x, 21 * value_turn_y, 0.0}};

    float texCoord[4][2] = { {0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
    shaderProgram.setUniformValue("color", QColor(100, 100, 100));
    shaderProgram.setAttributeArray("vertex", (float*)plane, 3);
    shaderProgram.enableAttributeArray("vertex");
    shaderProgram.setAttributeArray("texture", (float*)texCoord, 2);
    shaderProgram.enableAttributeArray("texture");
    glDrawArrays(GL_QUADS, 0, 4);
    shaderProgram.disableAttributeArray("vertex");
    shaderProgram.disableAttributeArray("texture");

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

