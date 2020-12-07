#ifndef READOBJ_H
#define READOBJ_H

#include <QVector>
#include <QVector3D>
#include <QPoint>
#include <QPolygon>
#include <iostream>
#include <QGLShaderProgram>

//Класс-парсер простых .obj файлов, которые содержат только координаты точек и треугольные грани без нормалей
class readObj
{
public:
    QVector<QVector3D> points; 		//массив точек
    QVector< QVector<int> > faces;  //массив граней, которые представляют собой позиции в массиве точек
    QString fileName;

    void addPoint(QString str);
    void addFace(QString str);

    readObj() {};
    readObj(QString fName);
    void readFile();
    void getVertexAndFaces(float vertex[][3], GLushort faces[]);	//получить данные об объекте, для его послед. отрисовки
    void printPoints();
};

#endif // READOBJ_H
