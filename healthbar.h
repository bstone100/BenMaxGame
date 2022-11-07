#ifndef HEALTHBAR_H
#define HEALTHBAR_H


#include <QGraphicsItem>

class Enemy;
class Player;

class HealthBar : public QGraphicsItem
{
public:
    enum Type {
        Moving,
        Still
    };

    HealthBar(Enemy *enemy = nullptr);
    HealthBar(Player *player = nullptr, Type t = Moving);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    const QRectF &getFullRect() const;

private:
    Enemy *enemy;
    Player *player;

    Type t;

    QRectF fullRect;
    QRectF healthRect;
};

#endif // HEALTHBAR_H
