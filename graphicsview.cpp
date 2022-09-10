#include "graphicsview.h"
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

    xAxis = new QGraphicsLineItem(0, scene->height() / 2, scene->width(), scene->height() / 2);
    scene->addItem(xAxis);
    yAxis = new QGraphicsLineItem(scene->width() / 2, 0, scene->width() / 2, scene->height());
    scene->addItem(yAxis);
    box = new QGraphicsRectItem(scene->sceneRect());
    scene->addItem(box);
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
    shotsTimer->start(50);
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
