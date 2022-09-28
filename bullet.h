#ifndef BULLET_H
#define BULLET_H

#include "QtCore/quuid.h"
#include <QGraphicsEllipseItem>

class Bullet : public QObject, public QGraphicsEllipseItem
{
public:
    Bullet();
    Bullet(QPointF gunTip, qreal angle);

    int getSize() const;
    void setSize(int newSize);

    int getDamage() const;
    void setDamage(int newDamage);

    void startExplosion();

    QPointF getGunTip() const;

    qreal getAngle() const;

    void setGunTip(QPointF newGunTip);

    void setAngle(qreal newAngle);

    QPointF getTempPos() const;

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    QPointF gunTip;
    qreal angle;

    int damage;

    bool dead;
    qreal scale;

    QPointF tempPos;
};

#endif // BULLET_H
