#include "healthbar.h"
#include "QtGui/qpainter.h"
#include "enemy.h"
#include "player.h"
#include "QGraphicsScene"

HealthBar::HealthBar(Enemy *enemy)
    : QGraphicsItem(enemy)
{
    this->enemy = enemy;
    player = NULL;
    fullRect = QRectF(0, 0, enemy->getSize(), 10);
    healthRect = QRectF(0, 0, 0, 10);
    setPos(0, enemy->getSize() + 10);
}

HealthBar::HealthBar(Player *player)
{
    this->player = player;
    enemy = NULL;
    fullRect = QRectF(0, 0, 200, 25);
    healthRect = QRectF(0, 0, 0, 25);
}

QRectF HealthBar::boundingRect() const
{
    return fullRect;
}

void HealthBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawRect(fullRect);


    painter->setBrush(QColorConstants::Svg::darkgreen);

    int health = 0;
    if (enemy) {
        health = enemy->getHealth();
    } else if (player) {
        health = (double)player->getHealth() / (double)player->getStartHealth() * fullRect.width();
    }
    healthRect.setWidth(health);

    if (health > 0)
        painter->drawRect(healthRect);
}



