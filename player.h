#ifndef PLAYER_H
#define PLAYER_H

#include "gun.h"
#include <QGraphicsRectItem>

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
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

    void resetProperties();

    const QString &getName() const;
    void setName(const QString &newName);

protected:
    void advance(int step) override;

signals:
    void moved(const QPointF &pos);

private:
    int size;
    int velo;
    bool up, down, left, right;
    Gun *gun;
    int health, startHealth;
    QString name;
};

#endif // PLAYER_H
