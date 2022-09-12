#ifndef HEALTHBAR_H
#define HEALTHBAR_H


#include <QGraphicsItem>

class Enemy;
class Player;

class HealthBar : public QGraphicsItem
{
public:
    HealthBar(Enemy *enemy = nullptr);
    HealthBar(Player *player = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

private:
    Enemy *enemy;
    Player *player;

    QRectF fullRect;
    QRectF healthRect;
};

#endif // HEALTHBAR_H
