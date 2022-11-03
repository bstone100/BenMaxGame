#include "data.h"
#include "QString"
#include "QDataStream"

Data::Data()
{

}

Data::Data(DataType type)
    : type(type)
{

}

QDataStream &operator<<(QDataStream &ds, const Data &data)
{
    ds << data.type;
    switch (data.type) {
    case Data::PlayerMove:
        ds << data.playerId;
        ds << data.playerPos;
        ds << data.playerMouseAngle;
        ds << data.playerHealth;
        ds << data.playerScore;
        break;
    case Data::NewPlayer:
        ds << data.playerId;
        ds << data.playerName;
        break;
    case Data::NewBullet:
        ds << data.bulletGunTip;
        ds << data.bulletAngle;
        break;
    case Data::NewEnemy:
        ds << data.enemyStartPoint;
        ds << data.enemyPlayerCenter;
        ds << data.enemyVelo;
        ds << data.enemySize;
        break;
    case Data::ServerFull:
        ds << data.serverSize;
        break;
    case Data::GameOver:
        break;
    }
    return ds;
}
QDataStream &operator>>(QDataStream &ds, Data &data)
{
    ds >> data.type;
    switch (data.type) {
    case Data::PlayerMove:
        ds >> data.playerId;
        ds >> data.playerPos;
        ds >> data.playerMouseAngle;
        ds >> data.playerHealth;
        ds >> data.playerScore;
        break;
    case Data::NewPlayer:
        ds >> data.playerId;
        ds >> data.playerName;
        break;
    case Data::NewBullet:
        ds >> data.bulletGunTip;
        ds >> data.bulletAngle;
        break;
    case Data::NewEnemy:
        ds >> data.enemyStartPoint;
        ds >> data.enemyPlayerCenter;
        ds >> data.enemyVelo;
        ds >> data.enemySize;
        break;
    case Data::ServerFull:
        ds >> data.serverSize;
        break;
    case Data::GameOver:
        break;
    }
    return ds;
}

QPointF Data::getPlayerPos() const
{
    return playerPos;
}

qreal Data::getPlayerMouseAngle() const
{
    return playerMouseAngle;
}

void Data::setPlayerPos(QPointF newPlayerPos)
{
    playerPos = newPlayerPos;
}

void Data::setPlayerMouseAngle(qreal newPlayerMouseAngle)
{
    playerMouseAngle = newPlayerMouseAngle;
}

qreal Data::getBulletAngle() const
{
    return bulletAngle;
}

void Data::setBulletAngle(qreal newBulletAngle)
{
    bulletAngle = newBulletAngle;
}

int Data::getEnemyVelo() const
{
    return enemyVelo;
}

void Data::setEnemyVelo(int newEnemyVelo)
{
    enemyVelo = newEnemyVelo;
}

QPointF Data::getBulletGunTip() const
{
    return bulletGunTip;
}

void Data::setBulletGunTip(QPointF newBulletGunTip)
{
    bulletGunTip = newBulletGunTip;
}

QPointF Data::getEnemyStartPoint() const
{
    return enemyStartPoint;
}

void Data::setEnemyStartPoint(QPointF newEnemyStartPoint)
{
    enemyStartPoint = newEnemyStartPoint;
}

QPointF Data::getEnemyPlayerCenter() const
{
    return enemyPlayerCenter;
}

void Data::setEnemyPlayerCenter(QPointF newEnemyPlayerCenter)
{
    enemyPlayerCenter = newEnemyPlayerCenter;
}

int Data::getPlayerHealth() const
{
    return playerHealth;
}

void Data::setPlayerHealth(int newPlayerHealth)
{
    playerHealth = newPlayerHealth;
}

int Data::getPlayerScore() const
{
    return playerScore;
}

void Data::setPlayerScore(int newPlayerScore)
{
    playerScore = newPlayerScore;
}

int Data::getEnemySize() const
{
    return enemySize;
}

void Data::setEnemySize(int newEnemySize)
{
    enemySize = newEnemySize;
}

Data::DataType Data::getType() const
{
    return type;
}

void Data::setType(DataType newType)
{
    type = newType;
}

const QUuid &Data::getPlayerId() const
{
    return playerId;
}

void Data::setPlayerId(const QUuid &newPlayerId)
{
    playerId = newPlayerId;
}

int Data::getServerSize() const
{
    return serverSize;
}

void Data::setServerSize(int newServerSize)
{
    serverSize = newServerSize;
}

const QString &Data::getPlayerName() const
{
    return playerName;
}

void Data::setPlayerName(const QString &newPlayerName)
{
    playerName = newPlayerName;
}
