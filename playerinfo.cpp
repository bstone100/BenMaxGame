#include "playerinfo.h"
#include "QtGui/qpainter.h"
#include "player.h"

PlayerInfo::PlayerInfo(Player *player)
    : QGraphicsItem(player), player(player)
{
    setPos(-player->getSize(), -player->getSize());
}

QRectF PlayerInfo::boundingRect() const
{
    return QRectF(0, 0, 3 * player->getSize(), player->getSize());
}

void PlayerInfo::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::gray);
    painter->setFont(QFont("Arial", 20, QFont::Bold));
    painter->drawText(boundingRect(), Qt::AlignVCenter | Qt::AlignHCenter , QString(player->getName()));
    painter->setFont(QFont("Arial", 15, QFont::Bold));
    painter->drawText(boundingRect(), Qt::AlignBottom | Qt::AlignHCenter, QString::number(player->getScore()));
}
