#include "data.h"
#include "QString"
#include "QDataStream"

Data::Data()
{

}

Data::Data(QPointF pos, double angle, int h, int s)
    : playerPos(pos), mouseAngle(angle), health(h), score(s)
{

}

QDataStream &Data::operator<<(QDataStream &ds)
{
    ds << playerPos << mouseAngle << health << score;
    return ds;
}

QDataStream &Data::operator >>(QDataStream &ds)
{
    ds >> playerPos >> mouseAngle >> health >> score;
    return ds;
}

QPointF Data::getPlayerPos() const
{
    return playerPos;
}

double Data::getMouseAngle() const
{
    return mouseAngle;
}

int Data::getHealth() const
{
    return health;
}
