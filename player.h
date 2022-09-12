#ifndef PLAYER_H
#define PLAYER_H

#include "gun.h"
#include <QGraphicsRectItem>

class Player : public QGraphicsPixmapItem
{
public:
    Player(QGraphicsItem *parent = nullptr);

    bool getUp() const;
    void setUp(bool newUp);

    bool getDown() const;
    void setDown(bool newDown);

    bool getLeft() const;
    void setLeft(bool newLeft);

    bool getRight() const;
    void setRight(bool newRight);

    int getSize() const;
    void setSize(int newSize);

    Gun *getGun() const;
    void setGun(Gun *newGun);

    int getHealth() const;
    void setHealth(int newHealth);

    int getStartHealth() const;
    void setStartHealth(int newStartHealth);

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    bool up, down, left, right;
    Gun *gun;
    int health, startHealth;
};

#endif // PLAYER_H
