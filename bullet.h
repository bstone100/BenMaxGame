#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsEllipseItem>

class Bullet : public QGraphicsEllipseItem
{
public:
    Bullet(QPointF gunTip, qreal angle);

    int getSize() const;
    void setSize(int newSize);

    int getDamage() const;
    void setDamage(int newDamage);

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    qreal angle;

    int damage;
};

#endif // BULLET_H
