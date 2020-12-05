#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QOpenGLWidget>
#include <gl/glu.h>
#include "brick.h"

const int CAMERA_SPEED = 3;

class MainGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MainGLWidget(QWidget *parent = nullptr);
    ~MainGLWidget();

    void setTurnXPos() { if (activeBrick != -1) { bricks[activeBrick]->setTurnXPos(); updateGL(); } };
    void setTurnXNeg() { if (activeBrick != -1) { bricks[activeBrick]->setTurnXNeg(); updateGL(); } };
    void setTurnYPos() { if (activeBrick != -1) { bricks[activeBrick]->setTurnYPos(); updateGL(); } };
    void setTurnYNeg() { if (activeBrick != -1) { bricks[activeBrick]->setTurnYNeg(); updateGL(); } };
    void setTurnZPos() { if (activeBrick != -1) { bricks[activeBrick]->setTurnZPos(); updateGL(); } };
    void setTurnZNeg() { if (activeBrick != -1) { bricks[activeBrick]->setTurnZNeg(); updateGL(); } };

    void setShiftXPos() { if (activeBrick != -1) { bricks[activeBrick]->setShiftXPos(); updateGL(); } };
    void setShiftXNeg() { if (activeBrick != -1) { bricks[activeBrick]->setShiftXNeg(); updateGL(); } };
    void setShiftYPos() { if (activeBrick != -1) { bricks[activeBrick]->setShiftYPos(); updateGL(); } };
    void setShiftYNeg() { if (activeBrick != -1) { bricks[activeBrick]->setShiftYNeg(); updateGL(); } };
    void setShiftZPos() { if (activeBrick != -1) { bricks[activeBrick]->setShiftZPos(); updateGL(); } };
    void setShiftZNeg() { if (activeBrick != -1) { bricks[activeBrick]->setShiftZNeg(); updateGL(); } };

    void addBrick(QString fileName);
    void addMyModel();
    void deleteActiveBrick() { if (activeBrick != -1) { bricks.removeAt(activeBrick); activeBrick = -1; updateGL(); } };

private:

    QVector<Brick*> bricks;
    PlaneAndAxis plane;
    int activeBrick;
    float deepOffset;
    QMatrix4x4 rotateMatrix; 						// Изначально матрица поворота равна единичной матрице

    QPointF mousePosition;							//позиция указателя мыши
    QVector3D sRayBegin;
    QVector3D sRayEnd;
    QGLShaderProgram shaderProgram;					// Сборщик шейдерных подпрограмм

    void textOut(void);								// Процедура выводит на экран текст (подсказку)
    void changeRotateMatrix(float dx, float dy);	// Процедура для изменения матрицы поворота
    void getWorldPos(QPoint screenPos);
    void setActiveBrick();

    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mouseMoveEvent(QMouseEvent* m_event);
    void wheelEvent(QWheelEvent* w_event);
    void mousePressEvent(QMouseEvent* m_event);

    signals:

};

#endif // MAINGLWIDGET_H
