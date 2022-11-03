#include "enemy.h"
#include "QRandomGenerator"
#include "QtGui/qbrush.h"
#include "QtWidgets/qgraphicsscene.h"
#include "healthbar.h"

int sizes[] = {30, 50, 70, 90, 110, 130, 150};

Enemy::Enemy(QPointF startPoint, QPointF playerCenter, int velo, int size)
{
    this->startPoint = startPoint;
    this->playerCenter = playerCenter;
    this->velo = velo;
    this->size = size;
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

    dead = false;
    scale = 1;
}

void Enemy::advance(int step)
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

bool Enemy::getDead() const
{
    return dead;
}

qreal Enemy::getAngle() const
{
    return angle;
}

void Enemy::setAngle(qreal newAngle)
{
    angle = newAngle;
}

const QPixmap &Enemy::getTempPix() const
{
    return tempPix;
}

QPointF Enemy::getTempPos() const
{
    return tempPos;
}

QPointF Enemy::getPlayerCenter() const
{
    return playerCenter;
}

QPointF Enemy::getStartPoint() const
{
    return startPoint;
}

int Enemy::getVelo() const
{
    return velo;
}

void Enemy::setVelo(int newVelo)
{
    velo = newVelo;
}

void Enemy::startExplosion()
{
    dead = true;
    healthBar->setVisible(false);
    setOpacity(.5);
    setTransformOriginPoint(boundingRect().center());
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
