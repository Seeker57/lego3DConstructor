#include "model.h"

void MyPolygon::deletePoint(Point point) {

    for(int i = 0; i < polygon.size(); i++)
        if (polygon[i] == point) {
            polygon.removeAt(i);
            break;
        }
}

void Model::deleteMyPolygon(MyPolygon polygon) {

    for(int i = 0; i < polygons.size(); i++)
        if (polygons[i] == polygon) {
            polygons.removeAt(i);
            break;
        }
}

void Model::draw(QPainter& painter, QPointF lastPoint) {

    QPen pen;
    pen.setWidth(1);
    painter.setPen(pen);

    int flagSelectedFace = 1;

    //для каждого полигона модели
    for(int index = indexMyPolygons.size() - 1; index >= 0; index--) {

        QPolygonF currentMyPolygon, invertMyPolygon;
        int realIndex = indexMyPolygons[index].first;
        int invertRealIndex = indexMyPolygons[indexMyPolygons.size() - 1 - index].first;

        //добавляем точки в текущий QMyPolygon
        for (int i = 0; i < polygons[realIndex].polygon.size(); i++) {
            currentMyPolygon << polygons[realIndex].polygon[i].toPoint();

            //добавляем точки в полигон, идущий с другого конца списка QMyPolygon
            for (int i = 0; i < polygons[invertRealIndex].polygon.size(); i++) {
                invertMyPolygon << polygons[invertRealIndex].polygon[i].toPoint();
            }

            if ( (flagSelectedFace == 1) && isSelectedFace(invertMyPolygon, lastPoint)) {
                    colorMyPolygons[invertRealIndex] = QColor::fromHsv(0, 255, 150);
                    flagSelectedFace = 0;
            }

        painter.setBrush(colorMyPolygons[realIndex]);
        painter.drawPolygon(currentMyPolygon);
        }
    }
}

bool Model::isSelectedFace(QPolygonF polygon, QPointF lastPoint) {

    return polygon.containsPoint(lastPoint, Qt::OddEvenFill);
}

Model::Model(int n) {

    //созданем грань шестиугольника
    MyPolygon hexagon;
    MyPolygon smallHexagon;
    float angle = 60.0 * 3.1415 / 180;
    float nextAngle = 3 * 3.1415 / 2;

    //стартовая точка - верх описанной около шестиугольника окружности
    QPointF startPoint = QPointF(radiusHexagon * cos(nextAngle), radiusHexagon * sin(nextAngle));
    QPointF smallStartPoint = QPointF(2.0 / 3.0 * radiusHexagon * cos(nextAngle), 2.0 / 3.0 * radiusHexagon * sin(nextAngle));
    hexagon.addPoint(Point(startPoint.x(), startPoint.y(), H1));
    smallHexagon.addPoint(Point(smallStartPoint.x(), smallStartPoint.y(), H1));

    for (int i = 1; i < 6; i++) {

       nextAngle += angle;
       startPoint.setX(radiusHexagon * cos(nextAngle));
       startPoint.setY(radiusHexagon * sin(nextAngle));
       smallStartPoint.setX(2.0 / 3.0 * radiusHexagon * cos(nextAngle));
       smallStartPoint.setY(2.0 / 3.0 * radiusHexagon * sin(nextAngle));
       hexagon.addPoint(Point(startPoint.x(), startPoint.y(), H1));
       smallHexagon.addPoint(Point(smallStartPoint.x(), smallStartPoint.y(), H1));
    }

    //триангулируем шестиугольник
    for (int i = 0; i < hexagon.howPoint(); i++) {

        int bPos, ePos;	//позиции начальной и конечной точки стороны
        if (i < (hexagon.howPoint() - 1)) {
            bPos = i;
            ePos = i + 1;
        }
        else {
            bPos = i;
            ePos = 0;
        }

        MyPolygon traingleHexagon;
        traingleHexagon.addPoint(Point(0.0, 0.0, H1));
        traingleHexagon.addPoint(smallHexagon[bPos]);
        traingleHexagon.addPoint(smallHexagon[ePos]);

        QVector3D midPoint = (traingleHexagon[1].point + traingleHexagon[2].point) / 2;
        MyPolygon smallTr1, smallTr2;

        smallTr1.addPoint(traingleHexagon[0]);
        smallTr1.addPoint(traingleHexagon[1]);
        smallTr1.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

        smallTr2.addPoint(traingleHexagon[0]);
        smallTr2.addPoint(traingleHexagon[2]);
        smallTr2.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

        addMyPolygon(smallTr1);
        addMyPolygon(smallTr2);

        QVector3D bigMidPoint = (hexagon[bPos].point + hexagon[ePos].point) / 2;
        MyPolygon smallTr3, smallTr4, smallTr5, smallTr6;

        smallTr3.addPoint(hexagon[bPos]);
        smallTr3.addPoint(smallTr1[1]);
        smallTr3.addPoint(Point(bigMidPoint.x(), bigMidPoint.y(), bigMidPoint.z()));

        smallTr4.addPoint(smallTr1[1]);
        smallTr4.addPoint(smallTr1[2]);
        smallTr4.addPoint(Point(bigMidPoint.x(), bigMidPoint.y(), bigMidPoint.z()));

        smallTr5.addPoint(smallTr2[1]);
        smallTr5.addPoint(smallTr2[2]);
        smallTr5.addPoint(Point(bigMidPoint.x(), bigMidPoint.y(), bigMidPoint.z()));

        smallTr6.addPoint(hexagon[ePos]);
        smallTr6.addPoint(smallTr2[1]);
        smallTr6.addPoint(Point(bigMidPoint.x(), bigMidPoint.y(), bigMidPoint.z()));

        MyPolygon smallTr7, smallTr8, smallTr9, smallTr10;

        QVector3D midPoint1 = (smallTr3[0].point + smallTr3[2].point) / 2;
        QVector3D midPoint2 = (smallTr6[0].point + smallTr6[2].point) / 2;

        smallTr7.addPoint(smallTr3[0]);
        smallTr7.addPoint(smallTr3[1]);
        smallTr7.addPoint(Point(midPoint1.x(), midPoint1.y(), midPoint1.z()));

        smallTr8.addPoint(smallTr3[1]);
        smallTr8.addPoint(smallTr3[2]);
        smallTr8.addPoint(Point(midPoint1.x(), midPoint1.y(), midPoint1.z()));

        smallTr9.addPoint(smallTr6[0]);
        smallTr9.addPoint(smallTr6[1]);
        smallTr9.addPoint(Point(midPoint2.x(), midPoint2.y(), midPoint2.z()));

        smallTr10.addPoint(smallTr6[1]);
        smallTr10.addPoint(smallTr6[2]);
        smallTr10.addPoint(Point(midPoint2.x(), midPoint2.y(), midPoint2.z()));

        addMyPolygon(smallTr7);
        addMyPolygon(smallTr8);
        addMyPolygon(smallTr9);
        addMyPolygon(smallTr10);

        addMyPolygon(smallTr4);
        addMyPolygon(smallTr5);
    }

    //создаем грань верхнего многоугольника
    MyPolygon topMyPolygon;
    for(int i = 0; i < hexagon.howPoint(); i++) {

        int bPos, ePos;	//позиции начальной и конечной точки стороны
        if (i < (hexagon.howPoint() - 1)) {
            bPos = i;
            ePos = i + 1;
        }
        else {
            bPos = i;
            ePos = 0;
        }

        QPointF midPoint; // середина текущей стороны шестиугольника
        midPoint.setX((hexagon.polygon[bPos].x() + hexagon.polygon[ePos].x() ) / 2.0);
        midPoint.setY((hexagon.polygon[bPos].y() + hexagon.polygon[ePos].y() ) / 2.0);

        QPointF leftMid; // точка левее середины отрезка
        leftMid.setX( (hexagon.polygon[bPos].x() + 3.0 * midPoint.x()) / (1 + 3.0));
        leftMid.setY( (hexagon.polygon[bPos].y() + 3.0 * midPoint.y()) / (1 + 3.0));

        QPointF rightMid; // точка правее середины отрезка
        rightMid.setX( (midPoint.x() + 1.0/3.0 * hexagon.polygon[ePos].x()) / (1 + 1.0/3.0));
        rightMid.setY( (midPoint.y() + 1.0/3.0 * hexagon.polygon[ePos].y()) / (1 + 1.0/3.0));

        topMyPolygon.addPoint(Point(leftMid.x(), leftMid.y(), 0.0));
        topMyPolygon.addPoint(Point(rightMid.x(), rightMid.y(), 0.0));
    }

    //триангулируем верхний многоугольник
    for (int i = 0; i < topMyPolygon.howPoint(); i++) {

        int bPos, ePos;	//позиции начальной и конечной точки стороны
        if (i < (topMyPolygon.howPoint() - 1)) {
            bPos = i;
            ePos = i + 1;
        }
        else {
            bPos = i;
            ePos = 0;
        }

        if (i % 2 == 0) {

            MyPolygon traingleHexagon;
            traingleHexagon.addPoint(Point(0.0, 0.0, 0.0));
            traingleHexagon.addPoint(topMyPolygon[bPos]);
            traingleHexagon.addPoint(topMyPolygon[ePos]);
            addMyPolygon(traingleHexagon);
        }
        else {
            MyPolygon smallTr1, smallTr2;
            QVector3D midPoint = (topMyPolygon[bPos].point + topMyPolygon[ePos].point) / 2;

            smallTr1.addPoint(Point(0.0, 0.0, 0.0));
            smallTr1.addPoint(topMyPolygon[bPos]);
            smallTr1.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

            smallTr2.addPoint(Point(0.0, 0.0, 0.0));
            smallTr2.addPoint(topMyPolygon[ePos]);
            smallTr2.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

            addMyPolygon(smallTr1);
            addMyPolygon(smallTr2);
        }
    }

    // добавляем треугольные и трапецивидные грани между верхним многоугольником и нижним шестиугольником
    // и триангулируем их
    for (int i = 0, k = 0; i < 6; i++, k += 2) {

        MyPolygon traingleFace;
        MyPolygon trapezeFace;

        // позиции точек в уже созданных полигонах
        int hexPoint1, hexPoint2, polPoint1, polPoint2, trPoint1, trPoint2;

        polPoint1 = k;
        polPoint2 = k + 1;
        if (i > 0 && i < 5) {

            hexPoint1 = i;
            hexPoint2 = i + 1;
            trPoint1 = k - 1;
            trPoint2 = k;

        }
        else if (i == 0) {

            hexPoint1 = i;
            hexPoint2 = i + 1;
            trPoint1 = 0;
            trPoint2 = 11;
        }
        else {

            hexPoint1 = i;
            hexPoint2 = 0;
            trPoint1 = k - 1;
            trPoint2 = k;
        }

        trapezeFace.addPoint(hexagon.polygon[hexPoint1]);
        trapezeFace.addPoint(topMyPolygon.polygon[polPoint1]);
        trapezeFace.addPoint(topMyPolygon.polygon[polPoint2]);
        trapezeFace.addPoint(hexagon.polygon[hexPoint2]);
        traingleFace.addPoint(hexagon.polygon[hexPoint1]);
        traingleFace.addPoint(topMyPolygon.polygon[trPoint1]);
        traingleFace.addPoint(topMyPolygon.polygon[trPoint2]);

        QVector3D midTraingle = (traingleFace[1].point + traingleFace[2].point) / 2;
        MyPolygon smallTr1, smallTr2;

        smallTr1.addPoint(traingleFace[0]);
        smallTr1.addPoint(traingleFace[1]);
        smallTr1.addPoint(Point(midTraingle.x(), midTraingle.y(), midTraingle.z()));

        smallTr2.addPoint(traingleFace[0]);
        smallTr2.addPoint(traingleFace[2]);
        smallTr2.addPoint(Point(midTraingle.x(), midTraingle.y(), midTraingle.z()));

        QVector3D midPoint1 = (smallTr1[0].point + smallTr1[1].point) / 2;
        QVector3D midPoint2 = (smallTr1[0].point + smallTr1[2].point) / 2;
        QVector3D midPoint3 = (smallTr2[0].point + smallTr2[1].point) / 2;
        MyPolygon smallTr3, smallTr4;

        smallTr3.addPoint(smallTr1[0]);
        smallTr3.addPoint(Point(midPoint1.x(), midPoint1.y(), midPoint1.z()));
        smallTr3.addPoint(Point(midPoint2.x(), midPoint2.y(), midPoint2.z()));

        smallTr4.addPoint(smallTr1[0]);
        smallTr4.addPoint(Point(midPoint2.x(), midPoint2.y(), midPoint2.z()));
        smallTr4.addPoint(Point(midPoint3.x(), midPoint3.y(), midPoint3.z()));

        addMyPolygon(smallTr3);
        addMyPolygon(smallTr4);

        MyPolygon smallTr5, smallTr6, smallTr7, smallTr8;

        smallTr5.addPoint(smallTr1[1]);
        smallTr5.addPoint(smallTr1[2]);
        smallTr5.addPoint(Point(midPoint1.x(), midPoint1.y(), midPoint1.z()));

        smallTr6.addPoint(smallTr1[2]);
        smallTr6.addPoint(Point(midPoint1.x(), midPoint1.y(), midPoint1.z()));
        smallTr6.addPoint(Point(midPoint2.x(), midPoint2.y(), midPoint2.z()));

        smallTr7.addPoint(smallTr1[2]);
        smallTr7.addPoint(Point(midPoint3.x(), midPoint3.y(), midPoint3.z()));
        smallTr7.addPoint(Point(midPoint2.x(), midPoint2.y(), midPoint2.z()));

        smallTr8.addPoint(smallTr2[1]);
        smallTr8.addPoint(smallTr2[2]);
        smallTr8.addPoint(Point(midPoint3.x(), midPoint3.y(), midPoint3.z()));

        addMyPolygon(smallTr5);
        addMyPolygon(smallTr6);
        addMyPolygon(smallTr7);
        addMyPolygon(smallTr8);

        MyPolygon traingleTrapeze1, traingleTrapeze2, traingleTrapeze3;

        QVector3D midPoint = (trapezeFace[0].point + trapezeFace[3].point) / 2;
        traingleTrapeze1.addPoint(trapezeFace[0]);
        traingleTrapeze1.addPoint(trapezeFace[1]);
        traingleTrapeze1.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

        traingleTrapeze2.addPoint(trapezeFace[1]);
        traingleTrapeze2.addPoint(trapezeFace[2]);
        traingleTrapeze2.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

        traingleTrapeze3.addPoint(trapezeFace[2]);
        traingleTrapeze3.addPoint(trapezeFace[3]);
        traingleTrapeze3.addPoint(Point(midPoint.x(), midPoint.y(), midPoint.z()));

        QVector3D midPoint4 = (traingleTrapeze1[0].point + traingleTrapeze1[1].point) / 2;
        QVector3D midPoint5 = (traingleTrapeze1[1].point + traingleTrapeze1[2].point) / 2;

        MyPolygon trTrapeze1, trTrapeze2, trTrapeze3;

        trTrapeze1.addPoint(traingleTrapeze1[1]);
        trTrapeze1.addPoint(Point(midPoint4.x(), midPoint4.y(), midPoint4.z()));
        trTrapeze1.addPoint(Point(midPoint5.x(), midPoint5.y(), midPoint5.z()));

        trTrapeze2.addPoint(traingleTrapeze1[0]);
        trTrapeze2.addPoint(traingleTrapeze1[2]);
        trTrapeze2.addPoint(Point(midPoint4.x(), midPoint4.y(), midPoint4.z()));

        trTrapeze3.addPoint(traingleTrapeze1[2]);
        trTrapeze3.addPoint(Point(midPoint5.x(), midPoint5.y(), midPoint5.z()));
        trTrapeze3.addPoint(Point(midPoint4.x(), midPoint4.y(), midPoint4.z()));

        addMyPolygon(trTrapeze1);
        addMyPolygon(trTrapeze3);

        QVector3D middlePoint1 = (trTrapeze2[0].point + trTrapeze2[1].point) / 2;
        MyPolygon smallTrTrapeze1, smallTrTrapeze2;

        smallTrTrapeze1.addPoint(trTrapeze2[0]);
        smallTrTrapeze1.addPoint(trTrapeze2[2]);
        smallTrTrapeze1.addPoint(Point(middlePoint1.x(), middlePoint1.y(), middlePoint1.z()));

        smallTrTrapeze2.addPoint(trTrapeze2[1]);
        smallTrTrapeze2.addPoint(trTrapeze2[2]);
        smallTrTrapeze2.addPoint(Point(middlePoint1.x(), middlePoint1.y(), middlePoint1.z()));

        addMyPolygon(smallTrTrapeze1);
        addMyPolygon(smallTrTrapeze2);

        QVector3D midPoint6 = (traingleTrapeze3[0].point + traingleTrapeze3[1].point) / 2;
        QVector3D midPoint7 = (traingleTrapeze3[0].point + traingleTrapeze3[2].point) / 2;

        MyPolygon trTrapeze4, trTrapeze5, trTrapeze6;

        trTrapeze4.addPoint(traingleTrapeze3[0]);
        trTrapeze4.addPoint(Point(midPoint6.x(), midPoint6.y(), midPoint6.z()));
        trTrapeze4.addPoint(Point(midPoint7.x(), midPoint7.y(), midPoint7.z()));

        trTrapeze5.addPoint(traingleTrapeze3[1]);
        trTrapeze5.addPoint(traingleTrapeze3[2]);
        trTrapeze5.addPoint(Point(midPoint6.x(), midPoint6.y(), midPoint6.z()));

        trTrapeze6.addPoint(traingleTrapeze3[2]);
        trTrapeze6.addPoint(Point(midPoint7.x(), midPoint7.y(), midPoint7.z()));
        trTrapeze6.addPoint(Point(midPoint6.x(), midPoint6.y(), midPoint6.z()));

        addMyPolygon(trTrapeze4);
        addMyPolygon(trTrapeze6);

        QVector3D middlePoint2 = (trTrapeze5[0].point + trTrapeze5[1].point) / 2;
        MyPolygon smallTrTrapeze3, smallTrTrapeze4;

        smallTrTrapeze3.addPoint(trTrapeze5[0]);
        smallTrTrapeze3.addPoint(trTrapeze5[2]);
        smallTrTrapeze3.addPoint(Point(middlePoint2.x(), middlePoint2.y(), middlePoint2.z()));

        smallTrTrapeze4.addPoint(trTrapeze5[1]);
        smallTrTrapeze4.addPoint(trTrapeze5[2]);
        smallTrTrapeze4.addPoint(Point(middlePoint2.x(), middlePoint2.y(), middlePoint2.z()));

        addMyPolygon(smallTrTrapeze3);
        addMyPolygon(smallTrTrapeze4);

        addMyPolygon(traingleTrapeze2);
    }

    //создаем нижнюю грань n-угольника
    MyPolygon nMyPolygonBottom;
    angle = (360.0 / n) * 3.1415 / 180;
    nextAngle = 3 * 3.1415 / 4;

    //стартовая точка - верх описанной около шестиугольника окружности
    startPoint = QPointF(radiusNMyPolygon * cos(nextAngle), radiusNMyPolygon * sin(nextAngle));
    nMyPolygonBottom.addPoint(Point(startPoint.x(), startPoint.y(), H1));

    for (int i = 1; i < n; i++) {

       nextAngle += angle;
       startPoint.setX(radiusNMyPolygon * cos(nextAngle));
       startPoint.setY(radiusNMyPolygon * sin(nextAngle));
       nMyPolygonBottom.addPoint(Point(startPoint.x(), startPoint.y(), H1));
    }

    //Триангулируем нижнюю грань n-угольника
    for (int i = 0; i < nMyPolygonBottom.howPoint(); i++) {

        int bPos, ePos;	//позиции начальной и конечной точки стороны
        if (i < (nMyPolygonBottom.howPoint() - 1)) {
            bPos = i;
            ePos = i + 1;
        }
        else {
            bPos = i;
            ePos = 0;
        }

        MyPolygon traingleHexagon;
        traingleHexagon.addPoint(Point(0.0, 0.0, H1));
        traingleHexagon.addPoint(nMyPolygonBottom[bPos]);
        traingleHexagon.addPoint(nMyPolygonBottom[ePos]);
        addMyPolygon(traingleHexagon);
    }

    //создаем верхнюю грань n-угольника
    MyPolygon nMyPolygonTop;
    angle = (360.0 / n) * 3.1415 / 180;
    nextAngle = 3 * 3.1415 / 4;

    //стартовая точка - верх описанной около шестиугольника окружности
    startPoint = QPointF(radiusNMyPolygon * cos(nextAngle), radiusNMyPolygon * sin(nextAngle));
    nMyPolygonTop.addPoint(Point(startPoint.x(), startPoint.y(), H2));

    for (int i = 1; i < n; i++) {

       nextAngle += angle;
       startPoint.setX(radiusNMyPolygon * cos(nextAngle));
       startPoint.setY(radiusNMyPolygon * sin(nextAngle));
       nMyPolygonTop.addPoint(Point(startPoint.x(), startPoint.y(), H2));
    }

    //триангулируем верхнюю грань n-угольника
    for (int i = 0; i < nMyPolygonTop.howPoint(); i++) {

        int bPos, ePos;	//позиции начальной и конечной точки стороны
        if (i < (nMyPolygonTop.howPoint() - 1)) {
            bPos = i;
            ePos = i + 1;
        }
        else {
            bPos = i;
            ePos = 0;
        }

        MyPolygon traingleHexagon;
        traingleHexagon.addPoint(Point(0.0, 0.0, H2));
        traingleHexagon.addPoint(nMyPolygonTop[bPos]);
        traingleHexagon.addPoint(nMyPolygonTop[ePos]);
        addMyPolygon(traingleHexagon);
    }

    // добавляем грани между верхним и нижнем многоугольником и  треугольные грани, сходящиеся в центре n-угольника
    // и так же трингулируем
    Point centerNMyPolygon(0.0, 0.0, H3);
    for (int i = 0; i < n; i++) {

        int pointPos1, pointPos2;
        if (i != n - 1) {
            pointPos1 = i;
            pointPos2 = i + 1;
         }
        else {
            pointPos1 = i;
            pointPos2 = 0;
        }

        MyPolygon nMyPolygonFace;
        nMyPolygonFace.addPoint(nMyPolygonBottom.polygon[pointPos1]);
        nMyPolygonFace.addPoint(nMyPolygonBottom.polygon[pointPos2]);
        nMyPolygonFace.addPoint(nMyPolygonTop.polygon[pointPos2]);
        nMyPolygonFace.addPoint(nMyPolygonTop.polygon[pointPos1]);

        QVector3D midRec = (nMyPolygonFace[0].point + nMyPolygonFace[2].point) / 2;
        MyPolygon traingleMyPolygonFace1, traingleMyPolygonFace2, traingleMyPolygonFace3, traingleMyPolygonFace4;

        traingleMyPolygonFace1.addPoint(nMyPolygonFace[0]);
        traingleMyPolygonFace1.addPoint(nMyPolygonFace[1]);
        traingleMyPolygonFace1.addPoint(Point(midRec.x(), midRec.y(), midRec.z()));

        traingleMyPolygonFace2.addPoint(nMyPolygonFace[1]);
        traingleMyPolygonFace2.addPoint(nMyPolygonFace[2]);
        traingleMyPolygonFace2.addPoint(Point(midRec.x(), midRec.y(), midRec.z()));

        traingleMyPolygonFace3.addPoint(nMyPolygonFace[2]);
        traingleMyPolygonFace3.addPoint(nMyPolygonFace[3]);
        traingleMyPolygonFace3.addPoint(Point(midRec.x(), midRec.y(), midRec.z()));

        traingleMyPolygonFace4.addPoint(nMyPolygonFace[3]);
        traingleMyPolygonFace4.addPoint(nMyPolygonFace[0]);
        traingleMyPolygonFace4.addPoint(Point(midRec.x(), midRec.y(), midRec.z()));

        addMyPolygon(traingleMyPolygonFace1);
        addMyPolygon(traingleMyPolygonFace2);
        addMyPolygon(traingleMyPolygonFace3);
        addMyPolygon(traingleMyPolygonFace4);

        MyPolygon traingleFace;
        traingleFace.addPoint(centerNMyPolygon);
        traingleFace.addPoint(nMyPolygonTop.polygon[pointPos1]);
        traingleFace.addPoint(nMyPolygonTop.polygon[pointPos2]);
        addMyPolygon(traingleFace);
    }
};

//возврат координаты z центра тяжести треугольной грани polygon
double Model::calcZ(MyPolygon polygon) {

    QVector3D centerG = (polygon[0].point + polygon[1].point + polygon[2].point) / 3.0;
    return centerG.z();
}

void Model::calcZBuffer() {

    int hsvColor = 0;
    //иниацилизируем будущий массив отсортированных граней парами(индекс грани, Z)
    //в кач-ве начального значения Z выберем максимально возможное целое INT_MAX
    for (int i = 0; i < polygons.size(); i++) {

        indexMyPolygons.push_back(make_pair(i, INT_MAX));
        colorMyPolygons.push_back(QColor::fromHsv(hsvColor % 360, 200, 150));
        hsvColor += 3;
    }

    //Вычисляем параметр Z для каждой грани
    for (int i = 0; i < polygons.size(); i++) {

        double currentZ = calcZ(polygons[i]);
        indexMyPolygons[i].second = currentZ;
    }
}

//ф-ция для сравнения при сортировке
bool compFunc(pair<int, double> a, pair<int, double> b) {

    return a.second < b.second;
}

void Model::sortMyPolygons() {

    sort(indexMyPolygons.begin(), indexMyPolygons.end(), compFunc);
}

void Model::getStaticModel(float vertex[][3]) {

    int i = 0;
    for (auto face : polygons) {

        vertex[i*3][0] = face[0].x();
        vertex[i*3][1] = face[0].y();
        vertex[i*3][2] = face[0].z();

        vertex[i*3 + 1][0] = face[1].x();
        vertex[i*3 + 1][1] = face[1].y();
        vertex[i*3 + 1][2] = face[1].z();

        vertex[i*3 + 2][0] = face[2].x();
        vertex[i*3 + 2][1] = face[2].y();
        vertex[i*3 + 2][2] = face[2].z();
        i++;
    }
}
