#include "data.h"
#include "QString"
#include "QDataStream"

Data::Data()
{

}

Data::Data(QString type)
    : type(type)
{

}

QDataStream &operator<<(QDataStream &ds, const Data &data)
{
    ds << data.type;
    QString t(data.type);
    if (t == "playerMove") {
        ds << data.playerPos;
        ds << data.playerMouseAngle;
        ds << data.playerHealth;
        ds << data.playerScore;
    } else if (t == "newBullet") {
        ds << data.bulletGunTip;
        ds << data.bulletAngle;
    } else if (t == "newEnemy") {
        ds << data.enemyStartPoint;
        ds << data.enemyPlayerCenter;
        ds << data.enemyVelo;
        ds << data.enemySize;
    }
    return ds;
}
QDataStream &operator>>(QDataStream &ds, Data &data)
{
    ds >> data.type;
    QString t(data.type);
    if (t == "playerMove") {
        ds >> data.playerPos;
        ds >> data.playerMouseAngle;
        ds >> data.playerHealth;
        ds >> data.playerScore;
    } else if (t == "newBullet") {
        ds >> data.bulletGunTip;
        ds >> data.bulletAngle;
    } else if (t == "newEnemy") {
        ds >> data.enemyStartPoint;
        ds >> data.enemyPlayerCenter;
        ds >> data.enemyVelo;
        ds >> data.enemySize;
    }
    return ds;
}

const QString &Data::getType() const
{
    return type;
}

void Data::setType(const QString &newType)
{
    type = newType;
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
