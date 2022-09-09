#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsEllipseItem>

class Bullet : public QGraphicsEllipseItem
{
public:
    Bullet(QPointF gunTip, qreal angle);

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    qreal angle;
};

#endif // BULLET_H
