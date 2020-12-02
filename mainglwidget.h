#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QOpenGLWidget>
#include <gl/glu.h>
#include "brick.h"

class MainGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MainGLWidget(QWidget *parent = nullptr);
    ~MainGLWidget();

    void setTurnXPos() { bricks[activeBrick]->setTurnXPos(); updateGL(); };
    void setTurnXNeg() { bricks[activeBrick]->setTurnXNeg(); updateGL(); };
    void setTurnYPos() { bricks[activeBrick]->setTurnYPos(); updateGL(); };
    void setTurnYNeg() { bricks[activeBrick]->setTurnYNeg(); updateGL(); };
    void setTurnZPos() { bricks[activeBrick]->setTurnZPos(); updateGL(); };
    void setTurnZNeg() { bricks[activeBrick]->setTurnZNeg(); updateGL(); };

    void setShiftXPos() { bricks[activeBrick]->setShiftXPos(); updateGL(); }
    void setShiftXNeg() { bricks[activeBrick]->setShiftXNeg(); updateGL(); }
    void setShiftYPos() { bricks[activeBrick]->setShiftYPos(); updateGL(); }
    void setShiftYNeg() { bricks[activeBrick]->setShiftYNeg(); updateGL(); }
    void setShiftZPos() { bricks[activeBrick]->setShiftZPos(); updateGL(); }
    void setShiftZNeg() { bricks[activeBrick]->setShiftZNeg(); updateGL(); }

    void addBrick(QString fileName);
    void addMyModel();

private:

    QVector<Brick*> bricks;
    int activeBrick = 0;
    float deepOffset;
    QMatrix4x4 rotateMatrix; 						// Изначально матрица поворота равна единичной матрице

    QPoint mousePosition;							//позиция указателя мыши
    QGLShaderProgram shaderProgram;					// Сборщик шейдерных подпрограмм

    void textOut(void);								// Процедура выводит на экран текст (подсказку)
    void changeRotateMatrix(float dx, float dy);	// Процедура для изменения матрицы поворота

    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mouseMoveEvent(QMouseEvent* m_event);
    void wheelEvent(QWheelEvent* w_event);
    void keyPressEvent(QKeyEvent* key_event);
    void mousePressEvent(QMouseEvent* m_event);

    signals:

};

#endif // MAINGLWIDGET_H
