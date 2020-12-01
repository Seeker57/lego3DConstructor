#ifndef BRICK_H
#define BRICK_H

#include <iostream>
#include <QGLWidget>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QOpenGLWidget>
#include <gl/glu.h>
#include "readobj.h"
#include "model.h"

const float VALUE_TURN_X = 0.0832;
const float VALUE_TURN_Y = 0.0532;
const float VALUE_TURN_Z = 0.0832;

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
    QMatrix4x4 modelViewMatrix; 	// Матрица видового преобразования
    QMatrix4x4 projectMatrix;		// Матрица проектирования

public:
    Brick() {};
    Brick(QString fileName);
    int howPoints() { return model.points.size(); };
    void getModels(float vertex[][3], GLushort faces[]) { model.getVertexAndFaces(vertex, faces); };
    void getWindowSize(int w, int h) { height = h, width = w; };

    void setTurnXPos() { xoffset -= VALUE_TURN_X; resetModelView(); };
    void setTurnXNeg() { xoffset += VALUE_TURN_X; resetModelView(); };
    void setTurnYPos() { yoffset -= VALUE_TURN_Y; resetModelView(); };
    void setTurnYNeg() { yoffset += VALUE_TURN_Y; resetModelView(); };
    void setTurnZPos() { zoffset -= VALUE_TURN_Z; resetModelView(); };
    void setTurnZNeg() { zoffset += VALUE_TURN_Z; resetModelView(); };

    void setShiftXPos() { rotateMatrix.rotate(90, 1, 0, 0); resetModelView(); }
    void setShiftXNeg() { rotateMatrix.rotate(-90, 1, 0, 0); resetModelView(); }
    void setShiftYPos() { rotateMatrix.rotate(90, 0, 1, 0); resetModelView(); }
    void setShiftYNeg() { rotateMatrix.rotate(-90, 0, 1, 0); resetModelView(); }
    void setShiftZPos() { rotateMatrix.rotate(90, 0, 0, 1); resetModelView(); }
    void setShiftZNeg() { rotateMatrix.rotate(-90, 0, 0, 1); resetModelView(); }

    void setDeepOffset(float newDeep) { deepOffset -= newDeep; resetModelView(); };
    void changeRotateMatrix(float dx, float dy);	// Процедура для изменения матрицы поворота

    void resetProjection();		// Процедура для изменения матрицы проектирования
    void resetModelView();		// Процедура для изменения видовой матрицы

    virtual void draw(QGLShaderProgram& shaderProgram);
};

class MyModel : public Brick {

public:
    MyModel();
    void draw(QGLShaderProgram& shaderProgram) override;
};

#endif // BRICK_H
