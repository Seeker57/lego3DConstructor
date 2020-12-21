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

void readObj::getVertexAndFaces(float vertex[][3], float normals[][3], float texCoords[][2]) {

    for (int i = 0; i < faces.size(); i++) {

        vertex[i*3][0] = points[faces[i][0]].x();
        vertex[i*3][1] = points[faces[i][0]].y();
        vertex[i*3][2] = points[faces[i][0]].z();

        vertex[i*3 + 1][0] = points[faces[i][1]].x();
        vertex[i*3 + 1][1] = points[faces[i][1]].y();
        vertex[i*3 + 1][2] = points[faces[i][1]].z();

        vertex[i*3 + 2][0] = points[faces[i][2]].x();
        vertex[i*3 + 2][1] = points[faces[i][2]].y();
        vertex[i*3 + 2][2] = points[faces[i][2]].z();

        QVector3D currentFaces[3];
        currentFaces[0] = QVector3D(points[faces[i][0]].x(), points[faces[i][0]].y(), points[faces[i][0]].z());
        currentFaces[1] = QVector3D(points[faces[i][1]].x(), points[faces[i][1]].y(), points[faces[i][1]].z());
        currentFaces[2] = QVector3D(points[faces[i][2]].x(), points[faces[i][2]].y(), points[faces[i][2]].z());

        QVector3D normal = QVector3D::normal(currentFaces[0], currentFaces[1], currentFaces[2]);
        normals[i*3][0] = normal.x();
        normals[i*3][1] = normal.y();
        normals[i*3][2] = normal.z();

        normals[i*3 + 1][0] = normal.x();
        normals[i*3 + 1][1] = normal.y();
        normals[i*3 + 1][2] = normal.z();

        normals[i*3 + 2][0] = normal.x();
        normals[i*3 + 2][1] = normal.y();
        normals[i*3 + 2][2] = normal.z();

        texCoords[i*3][0] = 0.0f;
        texCoords[i*3][1] = 0.0f;
        texCoords[i*3 + 1][0] = 1.0f;
        texCoords[i*3 + 1][1] = 0.0f;
        texCoords[i*3 + 2][0] = 0.5f;
        texCoords[i*3 + 2][1] = 1.0f;
    }
}
