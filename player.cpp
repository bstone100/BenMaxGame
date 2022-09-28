#include "player.h"
#include "QKeyEvent"
#include "QDebug"
#include "QtWidgets/qgraphicsscene.h"
#include <QUuid>

Player::Player(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    size = 50;
    setPixmap(QPixmap(":/images/bstone1oo.jpg"));

    velo = 5;

    up = false;
    down = false;
    left = false;
    right = false;

    gun = new Gun(this);

    health = startHealth = 500;
    healthBar = new HealthBar(this);

    name = "Ben";
    id = QUuid::createUuid();

    score = 0;
    mouseAngle = 0;
}

void Player::advance(int step)
{
    if (!step) return;

    if (left && x() > 0)
        moveBy(-velo, 0);
    if (right && x() < scene()->width() - size)
        moveBy(velo, 0);
    if (up && y() > 0)
        moveBy(0, -velo);
    if (down && y() < scene()->height() - size)
        moveBy(0, velo);
}

HealthBar *Player::getHealthBar() const
{
    return healthBar;
}

QPointF Player::getTempPos() const
{
    return tempPos;
}

qreal Player::getMouseAngle() const
{
    return mouseAngle;
}

void Player::setMouseAngle(qreal newMouseAngle)
{
    mouseAngle = newMouseAngle;
}

int Player::getScore() const
{
    return score;
}

void Player::setScore(int newScore)
{
    score = newScore;
}

const QUuid &Player::getId() const
{
    return id;
}

const QString &Player::getName() const
{
    return name;
}

void Player::setName(const QString &newName)
{
    name = newName;
}

int Player::getStartHealth() const
{
    return startHealth;
}

void Player::setStartHealth(int newStartHealth)
{
    startHealth = newStartHealth;
}

void Player::resetProperties()
{
    health = startHealth;
    up = down = left = right = false;
    gun->rotate(90);
}

int Player::getHealth() const
{
    return health;
}

void Player::setHealth(int newHealth)
{
    health = newHealth;
}

Gun *Player::getGun() const
{
    return gun;
}

void Player::setGun(Gun *newGun)
{
    gun = newGun;
}

int Player::getSize() const
{
    return size;
}

void Player::setSize(int newSize)
{
    size = newSize;
}

bool Player::getRight() const
{
    return right;
}

void Player::setRight(bool newRight)
{
    right = newRight;
}

bool Player::getLeft() const
{
    return left;
}

void Player::setLeft(bool newLeft)
{
    left = newLeft;
}

bool Player::getDown() const
{
    return down;
}

void Player::setDown(bool newDown)
{
    down = newDown;
}

bool Player::getUp() const
{
    return up;
}

void Player::setUp(bool newUp)
{
    up = newUp;
}








