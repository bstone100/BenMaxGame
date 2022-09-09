#ifndef PLAYER_H
#define PLAYER_H

#include "gun.h"
#include <QGraphicsRectItem>

class Player : public QGraphicsRectItem
{
public:
    Player();

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

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    bool up, down, left, right;
    Gun *gun;
};

#endif // PLAYER_H
