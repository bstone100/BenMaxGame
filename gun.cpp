#include "gun.h"

Gun::Gun(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    QPolygonF polygon;

    polygon << QPointF(65, 10);

    polygon << QPointF(65, 40);

    polygon << QPointF(85, 25);

    setPolygon(polygon);


    setTransformOriginPoint(25, 25);

}

void Gun::rotate(qreal angle)
{
    setRotation(-angle + 180);
}








