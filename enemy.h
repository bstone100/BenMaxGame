#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsRectItem>

class Enemy : public QGraphicsRectItem
{
public:
    Enemy(QPointF startPoint, QPointF playerCenter);

    int getSize() const;
    void setSize(int newSize);

    int getHealth() const;
    void setHealth(int newHealth);

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    qreal angle;

    int health;
    int startHealth;
};

#endif // ENEMY_H
