#include "graphicsview.h"
#include "QtCore/qtimer.h"
#include "QtGui/qevent.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    upPersistent = false;
    downPersistent = false;
    leftPersistent = false;
    rightPersistent = false;

    shotsTimer = new QTimer();
    QObject::connect(shotsTimer, &QTimer::timeout, this, &GraphicsView::shoot);

    delayTimer = new QTimer();
    delayTimer->setSingleShot(true);
    QObject::connect(delayTimer, &QTimer::timeout, this, &GraphicsView::startFullAuto);
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_A) {
        player->setLeft(true);
        leftPersistent = true;
        player->setRight(false);
    } else if (key == Qt::Key_Right || key == Qt::Key_D) {
        player->setRight(true);
        rightPersistent = true;
        player->setLeft(false);
    } else if (key == Qt::Key_Up || key == Qt::Key_W) {
        player->setUp(true);
        upPersistent = true;
        player->setDown(false);
    } else if (key == Qt::Key_Down || key == Qt::Key_S) {
        player->setDown(true);
        downPersistent = true;
        player->setUp(false);
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_A) {
        player->setLeft(false);
        leftPersistent = false;
        if (rightPersistent) player->setRight(true);
    } else if (key == Qt::Key_Right || key == Qt::Key_D) {
        player->setRight(false);
        rightPersistent = false;
        if (leftPersistent) player->setLeft(true);
    } else if (key == Qt::Key_Up || key == Qt::Key_W) {
        player->setUp(false);
        upPersistent = false;
        if (downPersistent) player->setDown(true);
    } else if (key == Qt::Key_Down || key == Qt::Key_S) {
        player->setDown(false);
        downPersistent = false;
        if (upPersistent) player->setUp(true);
    }
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    qreal mouseX = event->position().x();
    qreal mouseY = event->position().y();

    qreal centerX = player->x() + (player->getSize() / 2);
    qreal centerY = player->y() + (player->getSize() / 2);

    QLineF mouseLine(centerX, centerY, mouseX, mouseY);

    mouseAngle = mouseLine.angle();
    gunTip = mouseLine.pointAt(60 / mouseLine.length());

    player->getGun()->rotate(mouseAngle);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);

    pressedPersistent = true;
    shoot();
    delayTimer->start(250);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);

    pressedPersistent = false;
    shotsTimer->stop();
    delayTimer->stop();
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    scene()->setSceneRect(0, 0, width(), height());
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
    if (pressedPersistent) {
        qreal angleRadian(qDegreesToRadians(mouseAngle));
        // update gun tip when advance() called
        Bullet *bullet = new Bullet(gunTip, angleRadian);
        bullets.append(bullet);
        scene()->addItem(bullet);
    }
}

void GraphicsView::startFullAuto()
{
    shotsTimer->start(25);
}
