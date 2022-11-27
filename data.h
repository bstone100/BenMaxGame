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
    enum DataType {
        PlayerData,
        PlayerDirection,
        NewPlayer,
        NewBullet,
        NewEnemy,
        ServerFull,
        GameOver,
    };

    Data();
    Data(DataType type);

    friend QDataStream &operator<<(QDataStream &ds, const Data &data);
    friend QDataStream &operator>>(QDataStream &ds, Data &data);

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

    DataType getType() const;
    void setType(DataType newType);

    const QUuid &getPlayerId() const;
    void setPlayerId(const QUuid &newPlayerId);

    int getServerSize() const;
    void setServerSize(int newServerSize);

    const QString &getPlayerName() const;
    void setPlayerName(const QString &newPlayerName);

    bool getLeft() const;
    void setLeft(bool newLeft);

    bool getRight() const;
    void setRight(bool newRight);

    bool getDown() const;
    void setDown(bool newDown);

    bool getUp() const;
    void setUp(bool newUp);

private:
    DataType type;

    // general
    int serverSize;

    // player
    QUuid playerId;
    QString playerName;
    QPointF playerPos;
    qreal playerMouseAngle;
    int playerHealth;
    int playerScore;
    bool left, right, up, down;


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
