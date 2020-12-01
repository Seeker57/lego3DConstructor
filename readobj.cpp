#include "readobj.h"
#include <QFile>

readObj::readObj(QString fName) {
    fileName = fName;
}

void readObj::readFile() {

    QString nextStr;
    QFile myFile(fileName);
    if (!myFile.open(QIODevice::ReadOnly))
        qWarning("Files cannot find!");

    while (!myFile.atEnd()) {

        nextStr = myFile.readLine();
        if (nextStr[0] == 'v')
            addPoint(nextStr);
        else if (nextStr[0] == 'f')
            addFace(nextStr);
    }
    myFile.close();
}

void readObj::addPoint(QString str) {

    QStringList splitStr = str.split(' ');
    points.append(QVector3D(splitStr[1].toDouble(), splitStr[2].toDouble(), splitStr[3].toDouble()));
}

void readObj::addFace(QString str) {

    QStringList splitStr = str.split(' ');
    faces.append(QVector<int>());
    for (int i = 1; i < splitStr.size(); i++) {

        faces[faces.size() - 1].append(splitStr[i].toInt() - 1);
    }
}

void readObj::printPoints() {

    std::cout << std::endl;
    for (auto point : points) {
        for (int i = 0; i < 3; i++)
            std::cout << point[i] << ' ';
        std::cout << std::endl;
    }

    for (auto face : faces) {
        for (int i = 0; i < face.size(); i++)
            std::cout << face[i] << ' ';
        std::cout << std::endl;
    }
}

void readObj::getVertexAndFaces(float vertex[][3], GLushort face[]) {

    for (int i = 0; i < points.size(); i++) {

        vertex[i][0] = points[i].x();
        vertex[i][1] = points[i].y();
        vertex[i][2] = points[i].z();
    }

    for (int i = 0; i < faces.size(); i++) {

        face[i*3] = faces[i][0];
        face[i*3 + 1] = faces[i][1];
        face[i*3 + 2] = faces[i][2];
    }
}
