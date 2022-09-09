#include "player.h"
#include "QKeyEvent"
#include "QDebug"
#include "QtWidgets/qgraphicsscene.h"

Player::Player()
{
    size = 50;
    setRect(0, 0, size, size);

    velo = 5;

    up = false;
    down = false;
    left = false;
    right = false;

    gun = new Gun(this);
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








