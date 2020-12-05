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

const float VALUE_TURN_X = 0.0832;
const float VALUE_TURN_Y = 0.0832;
const float VALUE_TURN_Z = 0.0532;

class Brick
{

protected:
    readObj model;

    float xoffset;
    float yoffset;
    float zoffset;
    float deepOffset;

    int height;
    int width;

    QMatrix4x4 rotateMatrix; 		// Изначально матрица поворота равна единичной матрице
    QMatrix4x4 rotatePushMatrix;
    QMatrix4x4 modelViewMatrix; 	// Матрица видового преобразования
    QMatrix4x4 projectMatrix;		// Матрица проектирования

    int sgn(const float& k);

public:
    Brick() {};
    Brick(QString fileName, float deepOf, QMatrix4x4 R);
    int howPoints() { return model.points.size(); };
    void getModels(float vertex[][3], GLushort faces[]) { model.getVertexAndFaces(vertex, faces); };
    void getWindowSize(int w, int h) { height = h, width = w; };

    void setTurnXPos() { xoffset -= VALUE_TURN_X; resetModelView(); };
    void setTurnXNeg() { xoffset += VALUE_TURN_X; resetModelView(); };
    void setTurnYPos() { yoffset -= VALUE_TURN_Y; resetModelView(); };
    void setTurnYNeg() { yoffset += VALUE_TURN_Y; resetModelView(); };
    void setTurnZPos() { zoffset -= VALUE_TURN_Z; resetModelView(); };
    void setTurnZNeg() { zoffset += VALUE_TURN_Z; resetModelView(); };

    void setShiftXPos() { rotatePushMatrix.rotate(90, 1, 0, 0); resetModelView(); }
    void setShiftXNeg() { rotatePushMatrix.rotate(-90, 1, 0, 0); resetModelView(); }
    void setShiftYPos() { rotatePushMatrix.rotate(90, 0, 0, 1); resetModelView(); }
    void setShiftYNeg() { rotatePushMatrix.rotate(-90, 0, 0, 1); resetModelView(); }
    void setShiftZPos() { rotatePushMatrix.rotate(90, 0, 1, 0); resetModelView(); }
    void setShiftZNeg() { rotatePushMatrix.rotate(-90, 0, 1, 0); resetModelView(); }

    void setDeepOffset(float newDeep) { deepOffset = newDeep; resetModelView(); };
    void setRotateMatrix(QMatrix4x4 R) { rotateMatrix = R; };	// Процедура для изменения матрицы поворота
    bool virtual pointInBrick(QVector3D sRayBegin, QVector3D sRayEnd, float& minDepth);
    bool isCrossWithSL(QVector3D currentFaces[], QVector3D sRayBegin, QVector3D sRayEnd);

    void resetProjection();		// Процедура для изменения матрицы проектирования
    virtual void resetModelView();		// Процедура для изменения видовой матрицы

    virtual void draw(QGLShaderProgram& shaderProgram, bool isActive);
};

class MyModel : public Brick {

public:
    MyModel(float deepOf, QMatrix4x4 R);
    void draw(QGLShaderProgram& shaderProgram, bool isActive) override;
    bool pointInBrick(QVector3D worldPos1, QVector3D worldPos2, float& minDepth) override;
};

class PlaneAndAxis : public Brick {

public:
    PlaneAndAxis(float deepOf, QMatrix4x4 R);
    void draw(QGLShaderProgram& shaderProgram, bool isActive) override;
    void resetModelView() override;
};

#endif // BRICK_H
