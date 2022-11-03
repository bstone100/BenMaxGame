#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <QGraphicsItem>

class Player;

class PlayerInfo : public QGraphicsItem
{
public:
    PlayerInfo(Player *player = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
private:
    Player *player;
};

#endif // PLAYERINFO_H
