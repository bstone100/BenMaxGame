#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsEllipseItem>

class Bullet : public QObject, public QGraphicsEllipseItem
{
public:
    Bullet(QPointF gunTip, qreal angle);

    int getSize() const;
    void setSize(int newSize);

    int getDamage() const;
    void setDamage(int newDamage);

    void startExplosion();

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    qreal angle;

    int damage;

    bool dead;
    qreal scale;
};

#endif // BULLET_H
