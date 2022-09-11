#include "bullet.h"
#include "QtWidgets/qgraphicsscene.h"
#include "graphicsview.h"

Bullet::Bullet(QPointF gunTip, qreal angle)
{
    size = 8;
    velo = 10;
    this->angle = angle;
    setRect(0, 0, size, size);
    setPos(gunTip.x() - size / 2, gunTip.y() - size / 2);

    damage = 20;

//    setBrush(QBrush(Qt::green));
}

void Bullet::advance(int step)
{
    if (!step) return;

    moveBy(velo * cos(angle), -velo * sin(angle));
}

int Bullet::getDamage() const
{
    return damage;
}

void Bullet::setDamage(int newDamage)
{
    damage = newDamage;
}

int Bullet::getSize() const
{
    return size;
}

void Bullet::setSize(int newSize)
{
    size = newSize;
}
