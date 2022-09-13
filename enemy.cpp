#include "enemy.h"
#include "QRandomGenerator"
#include "QtGui/qbrush.h"
#include "healthbar.h"

int sizes[] = {30, 50, 70, 90, 110, 130, 150};

Enemy::Enemy(QPointF startPoint, QPointF playerCenter, int velo)
{
    this->velo = velo;

    size = sizes[QRandomGenerator::system()->bounded(7)];
    QPointF centerPoint(startPoint.x() + size / 2, startPoint.y() + size / 2);
    QLineF mouseLine(centerPoint, playerCenter);
    angle = qDegreesToRadians(mouseLine.angle());

    QString image(":/images/madmax/madmax");
    image += QString::number(size);
    image += ".jpg";
    setPixmap(QPixmap(image));
    setPos(startPoint);

    startHealth = health = damage = size;

    healthBar = new HealthBar(this);
}

void Enemy::advance(int step)
{
    if (!step) return;

    moveBy(velo * cos(angle), -velo * sin(angle));
}

int Enemy::getVelo() const
{
    return velo;
}

void Enemy::setVelo(int newVelo)
{
    velo = newVelo;
}

int Enemy::getStartHealth() const
{
    return startHealth;
}

void Enemy::setStartHealth(int newStartHealth)
{
    startHealth = newStartHealth;
}

int Enemy::getDamage() const
{
    return damage;
}

void Enemy::setDamage(int newDamage)
{
    damage = newDamage;
}

int Enemy::getHealth() const
{
    return health;
}

void Enemy::setHealth(int newHealth)
{
    health = newHealth;
}

int Enemy::getSize() const
{
    return size;
}

void Enemy::setSize(int newSize)
{
    size = newSize;
}
