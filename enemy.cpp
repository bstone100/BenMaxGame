#include "enemy.h"
#include "QRandomGenerator"
#include "QtGui/qbrush.h"

int sizes[] = {30, 50, 70, 90, 110, 130, 150};

Enemy::Enemy(QPointF startPoint, QPointF playerCenter)
{
    velo = 3;

    size = sizes[QRandomGenerator::system()->bounded(7)];
    QPointF centerPoint(startPoint.x() + size / 2, startPoint.y() + size / 2);
    QLineF mouseLine(centerPoint, playerCenter);
    angle = qDegreesToRadians(mouseLine.angle());

    setRect(0, 0, size, size);
    setPos(startPoint);

    startHealth = health = size;

//    setBrush(QBrush(Qt::red));
}

void Enemy::advance(int step)
{
    if (!step) return;

    moveBy(velo * cos(angle), -velo * sin(angle));
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
