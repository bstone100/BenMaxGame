#include "graphicsview.h"
#include "QtCore/qrandom.h"
#include "QtCore/qtimer.h"
#include "QtGui/qevent.h"
#include "healthbar.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setEnabled(false);

    mainTimer = new QTimer();
    QObject::connect(mainTimer, &QTimer::timeout, this, &GraphicsView::mainFunction);
    mainTimer->setInterval(10);

    shotsTimer = new QTimer();
    QObject::connect(shotsTimer, &QTimer::timeout, this, &GraphicsView::shoot);
    shotsTimer->setInterval(100);

    delayTimer = new QTimer();
    delayTimer->setSingleShot(true);
    QObject::connect(delayTimer, &QTimer::timeout, this, &GraphicsView::startFullAuto);
    delayTimer->setInterval(250);

    makeEnemyTimer = new QTimer();
    QObject::connect(makeEnemyTimer, &QTimer::timeout, this, &GraphicsView::generateEnemy);
    makeEnemyTimer->setInterval(200);

    player = new Player();
    playerHealthBar = new HealthBar(player);

    background = QPixmap(":/images/space3.jpg");

    title = new QGraphicsTextItem("BenMaxGame");
    title->setDefaultTextColor(Qt::white);
    scene->addItem(title);

    scoreText = new QGraphicsTextItem(QString::number(score));
    scoreText->setDefaultTextColor(Qt::white);
    scene->addItem(scoreText);
    scoreText->setVisible(false);
}

void GraphicsView::mainFunction()
{
    scene()->advance();
    moveGun();
    cleanUpScene();
    bulletImpact();
    enemyImpact();
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
    delayTimer->start();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);

    shotsTimer->stop();
    delayTimer->stop();
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QRectF newSceneRect(0, 0, width(), height());
    scene()->setSceneRect(newSceneRect);
    scene()->setBackgroundBrush(QBrush(background.scaled(width(), height())));

    setScene();

    title->setFont(QFont("Arial", width() / 8, QFont::Bold));
    title->setPos(width() / 2 - title->boundingRect().width() / 2, 30);

    scoreText->setFont(QFont("Arial", width() / 20, QFont::Bold));
    scoreText->setPos(30, height() - 100);

    QGraphicsView::resizeEvent(event);
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
    shotsTimer->start();
}

void GraphicsView::cleanUpScene()
{
    int size = 150;
    QRectF fullScene(-size, -size, scene()->width() + size * 2, scene()->height() + size * 2);
    for (int i = 0; i < bullets.size(); i++) {
        Bullet *bullet = bullets.at(i);
        QRectF bulletBorder(bullet->sceneBoundingRect());
        bool contains = fullScene.contains(bulletBorder);
        if (!contains) {
            bullets.remove(i);
            i--;
            delete bullet;
        }
    }

    for (int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        QRectF enemyBorder(enemy->sceneBoundingRect());
        bool contains = fullScene.contains(enemyBorder);
        if (!contains) {
            enemies.remove(i);
            i--;
            delete enemy;
        }
    }
}

void GraphicsView::generateEnemy()
{
    QPointF startPoint(QRandomGenerator::system()->bounded(scene()->width()), 0);
    Enemy *enemy = new Enemy(startPoint, playerCenter, enemyVelo);
    enemies.append(enemy);
    scene()->addItem(enemy);
}

void GraphicsView::bulletImpact()
{
    for (int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        QRectF enemyRect(enemy->sceneBoundingRect());
        for (int j = 0; j < bullets.size(); j++) {
            Bullet *bullet = bullets.at(j);
            QRectF bulletRect(bullet->sceneBoundingRect());
            if (enemyRect.intersects(bulletRect)) {
                enemy->setHealth(enemy->getHealth() - bullet->getDamage());
                bullets.remove(j);
                j--;
                delete bullet;
            }
        }
        if (enemy->getHealth() <= 0) {
            setScore(score + enemy->getStartHealth());
            enemies.remove(i);
            i--;
            delete enemy;
        }
    }
}

void GraphicsView::enemyImpact()
{
    QRectF playerRect(player->sceneBoundingRect());
    for (int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        QRectF enemyRect(enemy->sceneBoundingRect());
        if (enemyRect.intersects(playerRect)) {
            player->setHealth(player->getHealth() - enemy->getDamage());
            setScore(score + enemy->getStartHealth());
            if (player->getHealth() <= 0) {
                gameEnd();
                return;
            }
            playerHealthBar->update();
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

void GraphicsView::setScene()
{
    QRectF newSceneRect(0, 0, width(), height());
    QPointF center(newSceneRect.center());
    QPointF playerAdjust(player->getSize() / 2, player->getSize() / 2);
    player->setPos(center - playerAdjust);
    playerHealthBar->setPos(newSceneRect.width() / 2 - 100, newSceneRect.height() - 50);
    mouseTip = QPointF(width() / 2, 0);
}

void GraphicsView::gameStart()
{
    setScore(0);
    level = 0;
    enemyVelo = 2;
    upHeld = downHeld = leftHeld = rightHeld = false;
    player->resetProperties();
    setScene();

    scene()->removeItem(title);
    scene()->addItem(player);
    scene()->addItem(playerHealthBar);
    scoreText->setVisible(true);

    setEnabled(true);

    mainTimer->start();
    makeEnemyTimer->start();
}

void GraphicsView::gameEnd()
{
    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();

    scene()->removeItem(player);
    scene()->removeItem(playerHealthBar);
    scene()->addItem(title);

    setEnabled(false);

    mainTimer->stop();
    shotsTimer->stop();
    delayTimer->stop();
    makeEnemyTimer->stop();
}

void GraphicsView::setScore(int newScore)
{
    score = newScore;
    scoreText->setPlainText(QString::number(score));
    if (score >= level + 500) {
        level += 500;
        enemyVelo++;
    }
}


