#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsRectItem>

class HealthBar;

class Enemy : public QGraphicsPixmapItem
{
public:
    Enemy(QPointF startPoint, QPointF playerCenter, int velo);

    int getSize() const;
    void setSize(int newSize);

    int getHealth() const;
    void setHealth(int newHealth);

    int getDamage() const;
    void setDamage(int newDamage);

    int getStartHealth() const;
    void setStartHealth(int newStartHealth);

    int getVelo() const;
    void setVelo(int newVelo);

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    qreal angle;

    int health;
    int startHealth;
    HealthBar *healthBar;

    int damage;
};

#endif // ENEMY_H
