#ifndef DATA_H
#define DATA_H


#include "QtCore/qpoint.h"
#include <QVector>
#include "bullet.h"
#include "enemy.h"
#include "player.h"

class Data
{
public:
    Data();
    Data(QString type);

    friend QDataStream &operator<<(QDataStream &ds, const Data &data);
    friend QDataStream &operator>>(QDataStream &ds, Data &data);

    const QString &getType() const;
    void setType(const QString &newType);

    QPointF getPlayerPos() const;

    qreal getPlayerMouseAngle() const;

    void setPlayerPos(QPointF newPlayerPos);

    void setPlayerMouseAngle(qreal newPlayerMouseAngle);

    qreal getBulletAngle() const;
    void setBulletAngle(qreal newBulletAngle);

    int getEnemyVelo() const;
    void setEnemyVelo(int newEnemyVelo);

    QPointF getBulletGunTip() const;
    void setBulletGunTip(QPointF newBulletGunTip);

    QPointF getEnemyStartPoint() const;
    void setEnemyStartPoint(QPointF newEnemyStartPoint);

    QPointF getEnemyPlayerCenter() const;
    void setEnemyPlayerCenter(QPointF newEnemyPlayerCenter);

    int getPlayerHealth() const;
    void setPlayerHealth(int newPlayerHealth);

    int getPlayerScore() const;
    void setPlayerScore(int newPlayerScore);

    int getEnemySize() const;
    void setEnemySize(int newEnemySize);

private:
    QString type;

    // player
    QPointF playerPos;
    qreal playerMouseAngle;
    int playerHealth;
    int playerScore;


    // bullet
    QPointF bulletGunTip;
    qreal bulletAngle;

    // enemy
    QPointF enemyStartPoint;
    QPointF enemyPlayerCenter;
    int enemyVelo;
    int enemySize;
};

#endif // DATA_H
