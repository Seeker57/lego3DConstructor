#ifndef BRICK_H
#define BRICK_H

#include <QGLWidget>
#include <iostream>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QGLShaderProgram>
#include <QOpenGLWidget>
#include <gl/glu.h>
#include "readobj.h"
#include "model.h"

// константы для перемещения блоков в пространстве
const float VALUE_TURN_X = 0.0832;
const float VALUE_TURN_Y = 0.0832;
const float VALUE_TURN_Z = 0.0532;

/*Класс Блок, который является основным объектом для отрисовки на сцене, имеет виртуальные функции: отрисовки, изменения modelView,
а так же определения пересечения с селект. лучом, вледстсвие чего можно наследоваться от этого класса для реализации различных обеъектов*/
class Brick
{

protected:
    readObj model;	//объект считанный из .obj файла

    //координаты Блока в пространстве
    float xoffset;
    float yoffset;
    float zoffset;
    float deepOffset; 	//глубина объекта в пространстве

    //высота и ширина окна, на котором объект будет отрисован
    int height;
    int width;

    QMatrix4x4 rotateMatrix; 		// Матрица поворота, согласано повороту сцены
    QMatrix4x4 rotatePushMatrix;	// Матрица поворота для каждого блока отдельно
    QMatrix4x4 modelViewMatrix; 	// Матрица видового преобразования
    QMatrix4x4 projectMatrix;		// Матрица проектирования

    int sgn(const float& k);		//ф-ция определения знака числа (для нахождения пересечений граней блока с селект. лучом

public:
    Brick() {};
    Brick(QString fileName, float deepOf, QMatrix4x4 R);
    int howPoints() { return model.points.size(); };  //возвр. кол-во точек объекта
    void getModels(float vertex[][3], GLushort faces[]) { model.getVertexAndFaces(vertex, faces); }; //получить точки объекта и координаты его граней
    void getWindowSize(int w, int h) { height = h, width = w; }; //установить ширину и высоту окна, на котором будет производиться отрисовка

    //ф-ции для движения объекта по осям
    void setTurnXPos() { xoffset -= VALUE_TURN_X; resetModelView(); };
    void setTurnXNeg() { xoffset += VALUE_TURN_X; resetModelView(); };
    void setTurnYPos() { yoffset -= VALUE_TURN_Y; resetModelView(); };
    void setTurnYNeg() { yoffset += VALUE_TURN_Y; resetModelView(); };
    void setTurnZPos() { zoffset -= VALUE_TURN_Z; resetModelView(); };
    void setTurnZNeg() { zoffset += VALUE_TURN_Z; resetModelView(); };

    //ф-ции для поворота объекта по осям
    void setShiftXPos() { rotatePushMatrix.rotate(90, 1, 0, 0); resetModelView(); }
    void setShiftXNeg() { rotatePushMatrix.rotate(-90, 1, 0, 0); resetModelView(); }
    void setShiftYPos() { rotatePushMatrix.rotate(90, 0, 0, 1); resetModelView(); }
    void setShiftYNeg() { rotatePushMatrix.rotate(-90, 0, 0, 1); resetModelView(); }
    void setShiftZPos() { rotatePushMatrix.rotate(90, 0, 1, 0); resetModelView(); }
    void setShiftZNeg() { rotatePushMatrix.rotate(-90, 0, 1, 0); resetModelView(); }

    void setDeepOffset(float newDeep) { deepOffset = newDeep; resetModelView(); };	//установка значения глубины
    void setRotateMatrix(QMatrix4x4 R) { rotateMatrix = R; };	// Процедура для изменения матрицы поворота
    bool virtual pointInBrick(QVector3D sRayBegin, QVector3D sRayEnd, float& minDepth); //определяет пересекается ли Блок с селект. лучом
    bool isCrossWithSL(QVector3D currentFaces[], QVector3D sRayBegin, QVector3D sRayEnd); //определяет пересекается ли грань с селект. лучом

    void resetProjection();		        // Процедура для изменения матрицы проектирования
    virtual void resetModelView();		// Процедура для изменения видовой матрицы

    virtual void draw(QGLShaderProgram& shaderProgram, bool isActive); //ф-ция отрисовки Блока
};

//Класс модели, использовавшейся в 5 ЛР
class MyModel : public Brick {

public:
    MyModel(float deepOf, QMatrix4x4 R);
    void draw(QGLShaderProgram& shaderProgram, bool isActive) override;
    bool pointInBrick(QVector3D worldPos1, QVector3D worldPos2, float& minDepth) override;
};

//Класс для отрисовки сетки и координатных осей
class PlaneAndAxis : public Brick {

public:
    PlaneAndAxis(float deepOf, QMatrix4x4 R);
    void draw(QGLShaderProgram& shaderProgram, bool isActive) override;
    void resetModelView() override;
};

#endif // BRICK_H
