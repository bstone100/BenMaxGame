#include "bullet.h"
#include "QtWidgets/qgraphicsscene.h"
#include "game.h"

Bullet::Bullet(QPointF gunTip, qreal angle)
{
    size = 8;
    velo = 10;
    this->angle = angle;
    setRect(0, 0, size, size);
    setPos(gunTip.x() - size / 2, gunTip.y() - size / 2);

    damage = 20;

    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::yellow));

    dead = false;
    scale = 1;
}

void Bullet::advance(int step)
{
    if (!step) return;

    if (!dead) {
        moveBy(velo * cos(angle), -velo * sin(angle));
    } else {
        setScale(scale);
        if (scale >= 3) {
            deleteLater();
        } else {
            scale += .1;
        }
    }
}

int Bullet::getDamage() const
{
    return damage;
}

void Bullet::setDamage(int newDamage)
{
    damage = newDamage;
}

void Bullet::startExplosion()
{
    dead = true;
    setOpacity(1);
    setTransformOriginPoint(boundingRect().center());
}

int Bullet::getSize() const
{
    return size;
}

void Bullet::setSize(int newSize)
{
    size = newSize;
}
