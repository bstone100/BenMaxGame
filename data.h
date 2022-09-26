#ifndef DATA_H
#define DATA_H


#include "QtCore/qpoint.h"
#include <QVector>
class Data
{
public:
    Data();
    Data(QPointF pos, double angle, int h, int s);
    QDataStream &operator<<(QDataStream &ds);
    QDataStream &operator >> (QDataStream &ds);
    QPointF getPlayerPos() const;

    double getMouseAngle() const;

    int getHealth() const;

private:
    QPointF playerPos;
    double mouseAngle;
    int health;
    int score;
};

#endif // DATA_H
