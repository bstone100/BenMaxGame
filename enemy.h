#ifndef ENEMY_H
#define ENEMY_H

#include "QtCore/quuid.h"
#include <QGraphicsRectItem>
#include "timer.h"

class HealthBar;

class Enemy : public QObject, public QGraphicsPixmapItem
{
public:
    Enemy();
    Enemy(QPointF startPoint, QPointF playerCenter, int velo, int size);

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

    QPointF getStartPoint() const;

    QPointF getPlayerCenter() const;

    QPointF getTempPos() const;

    const QPixmap &getTempPix() const;

    qreal getAngle() const;
    void setAngle(qreal newAngle);

    bool getDead() const;

    void startHealthRegen();
    void healthRegen();
    void activateRegen();
    void pause();
    void resume();

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    qreal angle;

    QPointF startPoint;
    QPointF playerCenter;

    int health;
    int startHealth;
    HealthBar *healthBar;

    Timer *regenDelayTimer;
    Timer *regenTimer;

    int damage;

    bool dead;
    qreal scale;

    QPointF tempPos;
    QPixmap tempPix;
};

#endif // ENEMY_H
