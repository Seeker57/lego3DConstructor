#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <QOpenGLWidget>
#include <gl/glu.h>
#include "readobj.h"
#include "model.h"

class MainGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MainGLWidget(QWidget *parent = nullptr);
    void setFileName(QString fName) { fileName = fName; isBricks = true; updateGL(); }
    void drawMyModel() { isBricks = false; updateGL(); };

    void setTurnXPos() { xoffset -= 0.05; resetModelView(); updateGL(); };
    void setTurnXNeg() { xoffset += 0.05; resetModelView(); updateGL(); };
    void setTurnYPos() { yoffset -= 0.05; resetModelView(); updateGL(); };
    void setTurnYNeg() { yoffset += 0.05; resetModelView(); updateGL(); };
    void setTurnZPos() { zoffset2 -= 0.05; resetModelView(); updateGL(); };
    void setTurnZNeg() { zoffset2 += 0.05; resetModelView(); updateGL(); };

    void setShiftXPos() { rotateMatrix.rotate(90, 1, 0, 0); resetModelView(); updateGL(); }
    void setShiftXNeg() { rotateMatrix.rotate(-90, 1, 0, 0); resetModelView(); updateGL(); }
    void setShiftYPos() { rotateMatrix.rotate(90, 0, 1, 0); resetModelView(); updateGL(); }
    void setShiftYNeg() { rotateMatrix.rotate(-90, 0, 1, 0); resetModelView(); updateGL(); }
    void setShiftZPos() { rotateMatrix.rotate(90, 0, 0, 1); resetModelView(); updateGL(); }
    void setShiftZNeg() { rotateMatrix.rotate(-90, 0, 0, 1); resetModelView(); updateGL(); }

private:

    float xoffset;
    float yoffset;
    float zoffset2;
    float zoffset;					//глубина объекта
    QMatrix4x4 rotateMatrix; 		// Изначально матрица поворота равна единичной матрице
    QPoint mousePosition;			//позиция указателя мыши
    QMatrix4x4 modelViewMatrix; 	// Матрица видового преобразования
    QMatrix4x4 projectMatrix;		// Матрица проектирования
    QGLShaderProgram shaderProgram;	// Сборщик шейдерных подпрограмм
    QString fileName;				// Имя .obj файла
    bool isBricks;

    void resetProjection();		// Процедура для изменения матрицы проектирования
    void resetModelView();		// Процедура для изменения видовой матрицы
    static void changeRotateMatrix(QMatrix4x4& rotate_matrix, float dx, float dy);	// Процедура для изменения матрицы поворота
    void textOut(void);			// Процедура выводит на экран текст (подсказку)
    QColor randColor() { srand(time(NULL)); return QColor(rand() % 255, rand() % 255, rand() % 255); } 	//вернуть случайный цвет

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
