#include "bullet.h"
#include "QtWidgets/qgraphicsscene.h"
#include "game.h"
#include <QUuid>

Bullet::Bullet(QPointF gunTip, qreal angle)
{
    size = 8;
    velo = 10;
    this->gunTip = gunTip;
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

bool Bullet::getIsPrimaryBullet() const
{
    return isPrimaryBullet;
}

void Bullet::setIsPrimaryBullet(bool newIsPrimaryBullet)
{
    isPrimaryBullet = newIsPrimaryBullet;
}

QPointF Bullet::getTempPos() const
{
    return tempPos;
}

void Bullet::setAngle(qreal newAngle)
{
    angle = newAngle;
}

void Bullet::setGunTip(QPointF newGunTip)
{
    gunTip = newGunTip;
}

qreal Bullet::getAngle() const
{
    return angle;
}

QPointF Bullet::getGunTip() const
{
    return gunTip;
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
