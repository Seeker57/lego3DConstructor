#ifndef READOBJ_H
#define READOBJ_H

#include <QVector>
#include <QVector3D>
#include <QPoint>
#include <QPolygon>
#include <iostream>
#include <QGLShaderProgram>

class readObj
{
public:
    QVector<QVector3D> points; //массив точек
    QVector< QVector<int> > faces; //массив граней
    QString fileName;

    void addPoint(QString str);
    void addFace(QString str);

    readObj(QString fName);
    void readFile();
    void getVertexAndFaces(float vertex[][3], GLushort faces[]);
    void printPoints();
};

#endif // READOBJ_H
