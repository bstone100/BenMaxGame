#include "graphicsview.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qrandom.h"
#include "QtCore/qsettings.h"
#include "QtCore/qtimer.h"
#include "QtGui/qevent.h"
#include "QtNetwork/qtcpsocket.h"
#include "QtWidgets/qstyle.h"
#include "healthbar.h"
#include <QMessageBox>
#include <QInputDialog>

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    gameStarted = false;
    gamePaused = false;

    mainTimer = new QTimer();
    QObject::connect(mainTimer, &QTimer::timeout, this, &GraphicsView::mainFunction);
    mainTimer->setInterval(10);

    shotsTimer = new QTimer();
    QObject::connect(shotsTimer, &QTimer::timeout, this, &GraphicsView::shoot);
    shotsTimer->setInterval(50);

    delayTimer = new QTimer();
    delayTimer->setSingleShot(true);
    QObject::connect(delayTimer, &QTimer::timeout, this, &GraphicsView::startFullAuto);
    delayTimer->setInterval(250);

    makeEnemyTimer = new QTimer();
    QObject::connect(makeEnemyTimer, &QTimer::timeout, this, &GraphicsView::generateEnemy);
    makeEnemyTimer->setInterval(200);

    player = new Player();
    playerHealthBar = new HealthBar(player);
    playerHealthBar->setZValue(1);

    background = QPixmap(":/images/space3.jpg");

    title = new QGraphicsTextItem("BenMaxGame");
    title->setDefaultTextColor(Qt::white);
    scene->addItem(title);

    scoreText = new QGraphicsTextItem(QString::number(score));
    scoreText->setDefaultTextColor(Qt::white);
    scene->addItem(scoreText);
    scoreText->setVisible(false);
    scoreText->setZValue(1);

    QSettings settings("BenMax Productions", "BenMaxGame");
    highScore = 0;
    highScore = settings.value("highScore").toInt();
    highScoreText = new QGraphicsTextItem(QString::number(highScore));
    highScoreText->setDefaultTextColor(Qt::white);
    scene->addItem(highScoreText);

    playButton = new Button("Play");
    scene->addItem(playButton);
//    QObject::connect(playButton, &Button::clicked, chatWindow, &ChatWindow::attemptConnection);

    pauseButton = new Button("II");
    QObject::connect(pauseButton, &Button::clicked, this, &GraphicsView::gamePause);
    pauseButton->setRect(0, 0, 100, 100);
    pauseButton->setFontDivisor(2);
    pauseButton->setZValue(1);
//    pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPause));

    server = new ChatServer();
    serverButton = new Button("Start Server");
    serverButton->setRect(0, 0, 50, 50);
    serverButton->setFontDivisor(6);
    scene->addItem(serverButton);
    QObject::connect(serverButton, &Button::clicked, this, &GraphicsView::toggleStartServer);

    chatWindow = new ChatWindow();
    QObject::connect(chatWindow, &ChatWindow::readyToStart, this, &GraphicsView::gameStart);
    QObject::connect(chatWindow, &ChatWindow::playerJoined, this, &GraphicsView::addPlayer);
    QObject::connect(playButton, &Button::clicked, chatWindow, &ChatWindow::attemptConnection);

    QObject::connect(player, &Player::moved, chatWindow, &ChatWindow::sendMessage);
    QObject::connect(chatWindow, &ChatWindow::playerMoved, this, &GraphicsView::moveOtherPlayer);
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
    if (!gameStarted) return;

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
    } else if (key == Qt::Key_N) {
        qDeleteAll(enemies);
        enemies.clear();
    } else if (key == Qt::Key_Space) {
        pauseButton->mousePress(QPointF(pauseButton->sceneBoundingRect().center()));
        pauseButton->mouseRelease();
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (!gameStarted) return;

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
    if (!gameStarted) {
        playButton->mouseMove(event->position());
        serverButton->mouseMove(event->position());
        return;
    }

    pauseButton->mouseMove(event->position());

    if (gamePaused || pauseButton->getPressed()) return;

    mouseTip.setX(event->position().x());
    mouseTip.setY(event->position().y());

    moveGun();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        playButton->mousePress(event->position());
        serverButton->mousePress(event->position());
        return;
    }

    pauseButton->mousePress(event->position());

    if (gamePaused || pauseButton->getPressed()) return;

    mouseMoveEvent(event);

    shoot();
    delayTimer->start();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        playButton->mouseRelease();
        serverButton->mouseRelease();
        return;
    }

    pauseButton->mouseRelease();

    if (gamePaused || pauseButton->getPressed()) return;

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
    QRectF scoreAdjust = scoreText->boundingRect();
    scoreText->setPos(0, height() - scoreAdjust.height());

    highScoreText->setFont(QFont("Arial", width() / 20, QFont::Bold));
    QRectF highScoreAdjust = highScoreText->boundingRect();
    highScoreText->setPos(width() - highScoreAdjust.width(), height() - highScoreAdjust.height());

    playButton->setRect(0, 0, width() / 7, height() / 7);
    QPointF center(newSceneRect.center());
    QPointF playButtonAdjust(playButton->rect().center());
    playButton->setPos(center - playButtonAdjust);

    pauseButton->setRect(0, 0, width() / 20, width() / 20);
    QRectF pauseButtonAdjust = pauseButton->boundingRect();
    pauseButton->setPos(width() - pauseButtonAdjust.width(), height() - pauseButtonAdjust.height());

    serverButton->setRect(0, 0, width() / 8, height() / 9);
    QRectF serverButtonAdjust = serverButton->boundingRect();
    serverButton->setPos(width() - serverButtonAdjust.width(), 0);

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
                bullet->startExplosion();
            }
        }
        if (enemy->getHealth() <= 0) {
            setScore(score + enemy->getStartHealth());
            enemies.remove(i);
            i--;
            enemy->startExplosion();
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
            enemy->startExplosion();
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
    scene()->removeItem(playButton);
    scene()->removeItem(highScoreText);
    scene()->removeItem(serverButton);
    scene()->addItem(player);
    scene()->addItem(playerHealthBar);
    scene()->addItem(pauseButton);
    scoreText->setVisible(true);

    gameStarted = true;

    mainTimer->start();
    makeEnemyTimer->start();
}

void GraphicsView::gameEnd()
{
    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();

    // clean up any enemies still animating
    auto s = scene()->items();
    for (auto item: s) {
        Enemy *enemy = dynamic_cast<Enemy *>(item);
        if (enemy != NULL) {
            delete enemy;
            continue;
        }
        Bullet *bullet = dynamic_cast<Bullet *>(item);
        if (bullet != NULL) {
            delete bullet;
        }
    }

    scene()->removeItem(player);
    scene()->removeItem(playerHealthBar);
    scene()->removeItem(pauseButton);
    scene()->addItem(title);
    scene()->addItem(playButton);
    scene()->addItem(highScoreText);
    scene()->addItem(serverButton);

    gameStarted = false;

    mainTimer->stop();
    shotsTimer->stop();
    delayTimer->stop();
    makeEnemyTimer->stop();

    if (score > highScore) {
        highScore = score;
        QSettings settings("BenMax Productions", "BenMaxGame");
        settings.setValue("highScore", highScore);
        highScoreText->setPlainText(QString::number(highScore));
        QRectF highScoreAdjust = highScoreText->boundingRect();
        highScoreText->setPos(width() - highScoreAdjust.width(), height() - highScoreAdjust.height());
    }

    chatWindow->endGame();
}

void GraphicsView::gamePause()
{
    gamePaused = !gamePaused;
    if (gamePaused) {
        mainTimer->stop();
        shotsTimer->stop();
        delayTimer->stop();
        makeEnemyTimer->stop();
        pauseButton->setButtonName(">");
//        pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPlay));
    } else {
        mainTimer->start();
        makeEnemyTimer->start();
        pauseButton->setButtonName("II");
//        pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPause));
    }
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

// maybe have server window be a QDockWindow
void GraphicsView::toggleStartServer()
{
    if (server->isListening()) {
        server->stopServer();
        serverButton->setButtonName("Start Server");
    } else {
        if (!server->listen(QHostAddress::Any, 1967)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"));
            return;
        }
        serverButton->setButtonName("Stop Server");
    }
}

void GraphicsView::addPlayer()
{
    Player *newPlayer = new Player();
    HealthBar *newPlayerHealthbar = new HealthBar(newPlayer);
    newPlayerHealthbar->setZValue(1);
//    otherPlayers.append(newPlayer);
    otherPlayer = newPlayer;
    scene()->addItem(newPlayer);
    scene()->addItem(newPlayerHealthbar);

    QRectF newSceneRect(0, 0, width(), height());
    QPointF center(newSceneRect.center());
    QPointF playerAdjust(newPlayer->getSize() / 2, newPlayer->getSize() / 2);
    newPlayer->setPos(center - playerAdjust);
    newPlayerHealthbar->setPos(newSceneRect.width() / 2 + 100, newSceneRect.height() - 50);
}

void GraphicsView::moveOtherPlayer(QPointF pos)
{
    otherPlayer->setPos(pos);
}


