#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsRectItem>

class HealthBar;

class Enemy : public QObject, public QGraphicsPixmapItem
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

    void startExplosion();

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

    bool dead;
    qreal scale;
};

#endif // ENEMY_H
