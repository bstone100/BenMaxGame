#include "graphicsview.h"
#include "QtCore/qrandom.h"
#include "QtCore/qtimer.h"
#include "QtGui/qevent.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    upHeld = false;
    downHeld = false;
    leftHeld = false;
    rightHeld = false;

    shotsTimer = new QTimer();
    QObject::connect(shotsTimer, &QTimer::timeout, this, &GraphicsView::shoot);

    delayTimer = new QTimer();
    delayTimer->setSingleShot(true);
    QObject::connect(delayTimer, &QTimer::timeout, this, &GraphicsView::startFullAuto);

    cleanUpTimer = new QTimer();
    QObject::connect(cleanUpTimer, &QTimer::timeout, this, &GraphicsView::cleanUpScene);
    cleanUpTimer->start(10);

    bulletImpactTimer = new QTimer();
    QObject::connect(bulletImpactTimer, &QTimer::timeout, this, &GraphicsView::bulletImpact);
    bulletImpactTimer->start(10);

    makeEnemyTimer = new QTimer();
    QObject::connect(makeEnemyTimer, &QTimer::timeout, this, &GraphicsView::generateEnemy);
    makeEnemyTimer->start(500);


    xAxis = new QGraphicsLineItem(0, scene->height() / 2, scene->width(), scene->height() / 2);
    scene->addItem(xAxis);
    yAxis = new QGraphicsLineItem(scene->width() / 2, 0, scene->width() / 2, scene->height());
    scene->addItem(yAxis);
    box = new QGraphicsRectItem(scene->sceneRect());
    scene->addItem(box);

//    scene->setBackgroundBrush(QBrush(Qt::black));
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_A) {
        player->setLeft(true);
        leftHeld = true;
        player->setRight(false);
    } else if (key == Qt::Key_Right || key == Qt::Key_D) {
        player->setRight(true);
        rightHeld = true;
        player->setLeft(false);
    } else if (key == Qt::Key_Up || key == Qt::Key_W) {
        player->setUp(true);
        upHeld = true;
        player->setDown(false);
    } else if (key == Qt::Key_Down || key == Qt::Key_S) {
        player->setDown(true);
        downHeld = true;
        player->setUp(false);
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_A) {
        player->setLeft(false);
        leftHeld = false;
        if (rightHeld) player->setRight(true);
    } else if (key == Qt::Key_Right || key == Qt::Key_D) {
        player->setRight(false);
        rightHeld = false;
        if (leftHeld) player->setLeft(true);
    } else if (key == Qt::Key_Up || key == Qt::Key_W) {
        player->setUp(false);
        upHeld = false;
        if (downHeld) player->setDown(true);
    } else if (key == Qt::Key_Down || key == Qt::Key_S) {
        player->setDown(false);
        downHeld = false;
        if (upHeld) player->setUp(true);
    }
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    mouseTip.setX(event->position().x());
    mouseTip.setY(event->position().y());

    moveGun();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);

    shoot();
    delayTimer->start(250);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);

    shotsTimer->stop();
    delayTimer->stop();
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    scene()->setSceneRect(0, 0, width(), height());
    xAxis->setLine(0, scene()->height() / 2, scene()->width(), scene()->height() / 2);
    yAxis->setLine(scene()->width() / 2, 0, scene()->width() / 2, scene()->height());
    box->setRect(scene()->sceneRect());
//    qDebug() << width() << " by " << height();
    QGraphicsView::resizeEvent(event);
}

Player *GraphicsView::getPlayer() const
{
    return player;
}

void GraphicsView::setPlayer(Player *newPlayer)
{
    player = newPlayer;
}

void GraphicsView::shoot()
{
    qreal angleRadian(qDegreesToRadians(mouseAngle));
    Bullet *bullet = new Bullet(gunTip, angleRadian);
    bullets.append(bullet);
    scene()->addItem(bullet);
}

void GraphicsView::startFullAuto()
{
    shotsTimer->start(25);
}

// called on a timer
void GraphicsView::cleanUpScene()
{
    int size = 150;
    QRectF fullScene(-size, -size, scene()->width() + size * 2, scene()->height() + size * 2);

    // delete bullets off the scene
    for (int i = 0; i < bullets.size(); i++) {
        Bullet *bullet = bullets.at(i);
        QRectF bulletBorder(bullet->sceneBoundingRect());
        bool contains = fullScene.contains(bulletBorder);
        if (!contains) {
            scene()->removeItem(bullet);
            bullets.remove(i);
            i--;
            delete bullet;
        }
    }

    // delete enemies off the scene
    for (int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        QRectF enemyBorder(enemy->sceneBoundingRect());
        bool contains = fullScene.contains(enemyBorder);
        if (!contains) {
            scene()->removeItem(enemy);
            enemies.remove(i);
            i--;
            delete enemy;
        }
    }
}

void GraphicsView::generateEnemy()
{

    QPointF startPoint(QRandomGenerator::system()->bounded(scene()->width()), 0);
    Enemy *enemy = new Enemy(startPoint, playerCenter);
    enemies.append(enemy);
    scene()->addItem(enemy);
}

void GraphicsView::bulletImpact()
{
    for(int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        QRectF enemyRect(enemy->sceneBoundingRect());

        for(int j = 0; j < bullets.size(); j++) {
            Bullet *bullet = bullets.at(j);
            QRectF bulletRect(bullet->sceneBoundingRect());

            if (enemyRect.intersects(bulletRect)) {
                enemy->setHealth(enemy->getHealth() - bullet->getDamage());
                scene()->removeItem(bullet);
                bullets.remove(j);
                j--;
                delete bullet;
            }
        }
        if (enemy->getHealth() <= 0) {
            scene()->removeItem(enemy);
            enemies.remove(i);
            i--;
            delete enemy;
        }
    }
}

void GraphicsView::moveGun()
{
    playerCenter.setX(player->x() + (player->getSize() / 2));
    playerCenter.setY(player->y() + (player->getSize() / 2));

    QLineF mouseLine(playerCenter, mouseTip);

    mouseAngle = mouseLine.angle();
    gunTip = mouseLine.pointAt(60 / mouseLine.length());

    player->getGun()->rotate(mouseAngle);
}
