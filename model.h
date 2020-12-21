#ifndef MODEL_H
#define MODEL_H


#include <QVector>
#include <QVector3D>
#include <QPainter>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <climits>
#include <random>
#include <ctime>
#include "brick.h"

using namespace std;

const double radiusHexagon = 0.5;
const double radiusNMyPolygon = 0.3;
const double H1 = 0.15;
const double H2 = 0.575;
const double H3 = 0.725;

class Point {

    QVector3D point;
public:
    Point(double x, double y, double z) : point(x, y, z) {};
    double x() { return point.x(); };
    double y() { return point.y(); };
    double z() { return point.z(); };
    void setX(double x) { point.setX(x); };
    void setY(double x) { point.setY(x); };
    void setZ(double x) { point.setZ(x); };
    QPointF toPoint() { return point.toPointF(); };
    friend bool operator==(const Point point1, const Point point2) { return point1.point == point2.point; };

    friend class MyPolygon;
    friend class Model;
    friend class MyModel;
};

class MyPolygon {

    QVector<Point> polygon;
public:
    MyPolygon() {};
    MyPolygon(QVector<Point> points) : polygon(points) {};
    void addPoint(Point point) { polygon.push_back(point); };
    void deletePoint(Point point);
    int howPoint() const { return polygon.size(); };
    void viewPoints() { for (auto i : polygon) cout << i.x() << ", " << i.y() << ", " << i.z() << endl; };

    Point* begin() { return &polygon[0]; };
    const Point* begin() const { return &polygon[0]; };
    Point* end() { return &polygon[howPoint() - 1]; };
    const Point* end() const { return &polygon[howPoint() - 1]; };
    Point& operator[](int i) { return polygon[i]; };

    friend bool operator==(const MyPolygon polygon1, const MyPolygon polygon2) { return polygon1.polygon == polygon2.polygon; };

    friend class Model;
};

class Model {
    QVector<MyPolygon> polygons;
    QVector< pair<int, double> > indexMyPolygons;
    QVector<QColor> colorMyPolygons;

    double calcZ(MyPolygon polygon);
public:
    Model(int n);
    Model(QVector<MyPolygon> model) : polygons(model) {};
    void addMyPolygon(MyPolygon polygon) { polygons.push_back(polygon); };
    void deleteMyPolygon(MyPolygon polygon);
    int howPolygons() const { return polygons.size(); };
    void viewMyPolygons() { for (auto i : polygons) { i.viewPoints(); cout << endl; }};
    void viewIndexMyPolygons() { for (auto i : indexMyPolygons) { cout << i.first << " - " << i.second; cout << endl; } cout << endl; };

    MyPolygon* begin() { return &polygons[0]; };
    const MyPolygon* begin() const { return &polygons[0]; };
    MyPolygon* end() { return &polygons[howPolygons() - 1]; };
    const MyPolygon* end() const { return &polygons[howPolygons() - 1]; };
    MyPolygon& operator[](int i) { return polygons[i]; };

    void draw(QPainter& painter, QPointF lastPoint);
    void sortMyPolygons();
    void calcZBuffer();
    bool isSelectedFace(QPolygonF polygon, QPointF lastPoint);
    void getStaticModel(float vertex[][3], float normals[][3], float texCoords[][2]);
};

#endif // MODEL_H
