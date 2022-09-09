#ifndef GUN_H
#define GUN_H

#include <QGraphicsPolygonItem>

class Gun : public QGraphicsPolygonItem
{
public:
    Gun(QGraphicsItem *parent = nullptr);

    void rotate(qreal angle);
};

#endif // GUN_H
