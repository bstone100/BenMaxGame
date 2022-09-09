#include "graphicsview.h"
#include "QtGui/qevent.h"

GraphicsView::GraphicsView()
{
    // left and right can't be true at the same time
    // must remember that a key is still being held down even if another is pressed
    upPersistent = false;
    downPersistent = false;
    leftPersistent = false;
    rightPersistent = false;
}

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    upPersistent = false;
    downPersistent = false;
    leftPersistent = false;
    rightPersistent = false;
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

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    qreal mouseX = event->position().x();
    qreal mouseY = event->position().y();

    qreal centerX = player->x() + (player->getSize() / 2);
    qreal centerY = player->y() + (player->getSize() / 2);

    QLineF mouseLine(mouseX, mouseY, centerX, centerY);

    qreal angleDegree(mouseLine.angle());

    player->getGun()->rotate(angleDegree);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    qreal mouseX = event->position().x();
    qreal mouseY = event->position().y();

    qreal centerX = player->x() + (player->getSize() / 2);
    qreal centerY = player->y() + (player->getSize() / 2);

    QLineF mouseLine(mouseX, mouseY, centerX, centerY);

    qreal angleDegree(mouseLine.angle());
    angleDegree = angleDegree + 180;
    qreal angleRadian(qDegreesToRadians(angleDegree));

    QPointF gunTip = player->getGun()->scenePos();

    Bullet *bullet = new Bullet(gunTip, angleRadian);
    bullets.append(bullet);
    scene()->addItem(bullet);
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

Gun *GraphicsView::getGun() const
{
    return gun;
}

void GraphicsView::setGun(Gun *newGun)
{
    gun = newGun;
}
