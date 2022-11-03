#include "healthbar.h"
#include "QtGui/qpainter.h"
#include "enemy.h"
#include "player.h"

HealthBar::HealthBar(Enemy *enemy)
    : QGraphicsItem(enemy)
{
    this->enemy = enemy;
    player = NULL;
    fullRect = QRectF(0, 0, enemy->getSize(), 5);
    healthRect = QRectF(0, 0, 0, 5);
    setPos(0, enemy->getSize() + 10);
}

HealthBar::HealthBar(Player *player, Type t)
{
    this->player = player;
    enemy = NULL;

    switch (t) {
    case Moving:
        setParentItem(player);
        fullRect = QRectF(0, 0, player->getSize(), 5);
        healthRect = QRectF(0, 0, 0, 5);
        setPos(0, player->getSize() + 10);
        break;
    case Still:
        fullRect = QRectF(0, 0, 200, 25);
        healthRect = QRectF(0, 0, 0, 25);
    }
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

    int health = 0;
    if (enemy) {
        health = enemy->getHealth();
    } else if (player) {
        health = (double)player->getHealth() / (double)player->getStartHealth() * fullRect.width();
    }
    healthRect.setWidth(health);

    double percent = healthRect.width() / fullRect.width();
    if (percent >= .2) {
        painter->setBrush(QColorConstants::Svg::darkgreen);
    } else {
        painter->setBrush(Qt::red);
    }

    if (health > 0)
        painter->drawRect(healthRect);
}

const QRectF &HealthBar::getFullRect() const
{
    return fullRect;
}



