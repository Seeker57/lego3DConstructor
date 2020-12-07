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

    //ф-ции для передвижения выбранного блока по осям
    void setTurnXPos() { if (activeBrick != -1) { bricks[activeBrick]->setTurnXPos(); updateGL(); } };
    void setTurnXNeg() { if (activeBrick != -1) { bricks[activeBrick]->setTurnXNeg(); updateGL(); } };
    void setTurnYPos() { if (activeBrick != -1) { bricks[activeBrick]->setTurnYPos(); updateGL(); } };
    void setTurnYNeg() { if (activeBrick != -1) { bricks[activeBrick]->setTurnYNeg(); updateGL(); } };
    void setTurnZPos() { if (activeBrick != -1) { bricks[activeBrick]->setTurnZPos(); updateGL(); } };
    void setTurnZNeg() { if (activeBrick != -1) { bricks[activeBrick]->setTurnZNeg(); updateGL(); } };

    //ф-ции для поворота выбранного блока по осям
    void setShiftXPos() { if (activeBrick != -1) { bricks[activeBrick]->setShiftXPos(); updateGL(); } };
    void setShiftXNeg() { if (activeBrick != -1) { bricks[activeBrick]->setShiftXNeg(); updateGL(); } };
    void setShiftYPos() { if (activeBrick != -1) { bricks[activeBrick]->setShiftYPos(); updateGL(); } };
    void setShiftYNeg() { if (activeBrick != -1) { bricks[activeBrick]->setShiftYNeg(); updateGL(); } };
    void setShiftZPos() { if (activeBrick != -1) { bricks[activeBrick]->setShiftZPos(); updateGL(); } };
    void setShiftZNeg() { if (activeBrick != -1) { bricks[activeBrick]->setShiftZNeg(); updateGL(); } };

    void addBrick(QString fileName);	//добавить на сцену блок, содержащийся в файле fileName.obj
    void addMyModel();					//Добавить на сцену модель из 5 ЛР
    void deleteActiveBrick() { if (activeBrick != -1) { bricks.removeAt(activeBrick); activeBrick = -1; updateGL(); } };	//удалить выбранный блок

private:

    QVector<Brick*> bricks;							// все объекты на сцене
    PlaneAndAxis plane;								// объект сетки и координатных осей
    int activeBrick;								// позиция выбранного объекта в векторе объектов
    float deepOffset;								// значений глубины для всей сцены
    QMatrix4x4 rotateMatrix; 						// матрица поворота для всей сцены

    QPointF mousePosition;							// позиция указателя мыши
    QVector3D sRayBegin;							// точка начала селект. луча
    QVector3D sRayEnd;								// точка конца селект. луча
    QGLShaderProgram shaderProgram;					// Сборщик шейдерных подпрограмм

    void textOut(void);								// Процедура выводит на экран текст (подсказку)
    void changeRotateMatrix(float dx, float dy);	// Процедура для изменения матрицы поворота
    void getWorldPos(QPoint screenPos);				// Процедура для определения мировых координат селект. луча в зависимости от позиции мыши на экране
    void setActiveBrick();							// Процедура выбора активного объекта в зависимости от перемечения с селект. лучом

    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mouseMoveEvent(QMouseEvent* m_event);
    void wheelEvent(QWheelEvent* w_event);
    void mousePressEvent(QMouseEvent* m_event);

    signals:

};

#endif // MAINGLWIDGET_H
