#ifndef PLAYER_H
#define PLAYER_H

#include "QtCore/quuid.h"
#include "gun.h"
#include "healthbar.h"
#include <QGraphicsRectItem>

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Player(QUuid id);

    bool getUp() const;
    void setUp(bool newUp);

    bool getDown() const;
    void setDown(bool newDown);

    bool getLeft() const;
    void setLeft(bool newLeft);

    bool getRight() const;
    void setRight(bool newRight);

    int getSize() const;
    void setSize(int newSize);

    Gun *getGun() const;
    void setGun(Gun *newGun);

    int getHealth() const;
    void setHealth(int newHealth);

    int getStartHealth() const;
    void setStartHealth(int newStartHealth);

    void resetProperties();

    const QString &getName() const;
    void setName(const QString &newName);

    const QUuid &getId() const;

    int getScore() const;
    void setScore(int newScore);

    qreal getMouseAngle() const;
    void setMouseAngle(qreal newMouseAngle);

    QPointF getTempPos() const;

    HealthBar *getHealthBar() const;

    void setId(const QUuid &newId);

    void startExplosion();

    bool getDead() const;

protected:
    void advance(int step) override;

private:
    int size;
    int velo;
    bool up, down, left, right;
    Gun *gun;
    int health, startHealth;
    HealthBar *healthBar;
    int score;
    qreal mouseAngle;
    QString name;
    QPointF tempPos;

    bool dead;
    qreal scale;

    QUuid id;
};

#endif // PLAYER_H
