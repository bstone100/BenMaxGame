#include "bullet.h"

Bullet::Bullet(QPointF gunTip, qreal angle)
{
    size = 8;
    velo = 10;
    this->angle = angle;
    setRect(gunTip.x(), gunTip.y(), 8, 8);
}

void Bullet::advance(int step)
{
    if (!step) return;

    // need radians
    moveBy(velo * cos(angle), -velo * sin(angle));
}
