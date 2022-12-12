#include "game.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qrandom.h"
#include "QtCore/qsettings.h"
#include "QtCore/qtimer.h"
#include "QtGui/qevent.h"
#include "QtNetwork/qtcpsocket.h"
#include "QtWidgets/qapplication.h"
#include "QtWidgets/qstyle.h"
#include "healthbar.h"
#include <QMessageBox>
#include <QInputDialog>
#include "data.h"
#include "serverworker.h"

int enemySizes[] = {30, 50, 70, 90, 110, 130, 150};

Game::Game(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    Enemy::makeImages();

    gameStarted = false;
    gamePaused = false;
    justResumed = false;
    autoFire = false;

    serverSize = 0;

    // calculations happen 100 times per second
    mainTimer = new Timer(this, 10);
    QObject::connect(mainTimer, &Timer::timeout, this, &Game::mainFunction);

    sendDataTimer = new Timer(this, 10);
    QObject::connect(sendDataTimer, &Timer::timeout, this, &Game::sendPlayerData);

    shotsTimer = new Timer(this, 50);
    QObject::connect(shotsTimer, &Timer::timeout, this, &Game::shoot);

    delayTimer = new Timer(this, 250, true);
    QObject::connect(delayTimer, &Timer::timeout, this, &Game::startFullAuto);

    makeEnemyTimer = new Timer(this, 200);
    QObject::connect(makeEnemyTimer, &Timer::timeout, this, &Game::generateEnemy);

    fpsTimer = new Timer(this, 100);
    QObject::connect(fpsTimer, &Timer::timeout, this, &Game::setFps);

    fpsStopwatch = new QElapsedTimer();


    fpsText = new QGraphicsTextItem();
    fpsText->setDefaultTextColor(Qt::white);
    fpsText->setFont(QFont("Menlo", scene->width() / 75, QFont::Bold));
    fpsText->setPos(0, 0);
    fpsText->setZValue(1);

    regenDelayTimer = new Timer(this, 500, true);
    QObject::connect(regenDelayTimer, &Timer::timeout, this, &Game::startHealthRegen);

    regenTimer = new Timer(this, 10);
    QObject::connect(regenTimer, &Timer::timeout, this, &Game::healthRegen);


    QSettings settings("BenMax Productions", "BenMaxGame");
    player = new Player(QUuid::createUuid(), settings.value("name").toString());
    playerHealthBar = new HealthBar(player, HealthBar::Still);
    playerHealthBar->setPos(scene->width() / 2 - playerHealthBar->getFullRect().width() / 2,
                            scene->height() - playerHealthBar->getFullRect().height() * 2);
    playerHealthBar->setZValue(1);

    background = QPixmap(":/images/space3.jpg");

    title = new QGraphicsTextItem("BenMaxGame");
    title->setDefaultTextColor(Qt::white);
    title->setFont(QFont("Arial", scene->width() / 8, QFont::Bold));
    title->setPos(scene->width() / 2 - title->boundingRect().width() / 2, scene->height() / 15);
    scene->addItem(title);

    scoreText = new QGraphicsTextItem(QString::number(player->getScore()));
    scoreText->setDefaultTextColor(Qt::white);
    scoreText->setFont(QFont("Arial", scene->width() / 20, QFont::Bold));
    QRectF scoreAdjust = scoreText->boundingRect();
    scoreText->setPos(0, scene->height() - scoreAdjust.height());
    scene->addItem(scoreText);
    scoreText->setVisible(false);
    scoreText->setZValue(1);

    highScore = 0;
    highScore = settings.value("highScore").toInt();
    highScoreText = new QGraphicsTextItem(QString::number(highScore));
    highScoreText->setDefaultTextColor(Qt::white);
    highScoreText->setFont(QFont("Arial", scene->width() / 20, QFont::Bold));
    QRectF highScoreAdjust = highScoreText->boundingRect();
    highScoreText->setPos(scene->width() - highScoreAdjust.width(), scene->height() - highScoreAdjust.height());
    scene->addItem(highScoreText);

    pauseButton = new Button("II", ">");
    QObject::connect(pauseButton, &Button::clicked, this, &Game::gamePause);
    pauseButton->setRect(0, 0, scene->width() / 20, scene->width() / 20);
    QRectF pauseButtonAdjust = pauseButton->boundingRect();
    pauseButton->setPos(scene->width() - pauseButtonAdjust.width(), scene->height() - pauseButtonAdjust.height());
    pauseButton->setFontDivisor(2);
    pauseButton->setZValue(1);
//    pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPause));

    changeNameButton = new Button("Change\nName", "Change\nName");
    scene->addItem(changeNameButton);
    QObject::connect(changeNameButton, &Button::clicked, this, &Game::changeName);
    changeNameButton->setFontDivisor(5);

    startLocalGame = new Button("Play", "Play");
    scene->addItem(startLocalGame);
    QObject::connect(startLocalGame, &Button::clicked, this, &Game::startSoloGame);

    startPublicGame = new Button("Start\nServer", "Stop\nServer", true);
    scene->addItem(startPublicGame);
    startPublicGame->setFontDivisor(5);
    QObject::connect(startPublicGame, &Button::clicked, this, &Game::toggleStartServer);

    joinPublicGame = new Button("Connect", "Disconnect", true);
    scene->addItem(joinPublicGame);
    joinPublicGame->setFontDivisor(6);
    QObject::connect(joinPublicGame, &Button::clicked, this, &Game::attemptConnection);

    buttons = QVector<Button *>{startLocalGame, startPublicGame, joinPublicGame, changeNameButton};


    QPointF center(scene->sceneRect().center());

    // center
    startLocalGame->setRect(0, 0, scene->width() / 6, scene->height() / 6);
    QPointF localGameAdjust(startLocalGame->rect().center());
    startLocalGame->setPos(center - localGameAdjust);

    // left of center
    startPublicGame->setRect(0, 0, scene->width() / 7, scene->height() / 7);
    QPointF publicGameAdjust(startPublicGame->rect().center());
    startPublicGame->setPos(center - publicGameAdjust - QPointF(startLocalGame->rect().width() + 20, 0));

    // right of center
    joinPublicGame->setRect(0, 0, scene->width() / 7, scene->height() / 7);
    QPointF joinPublicAdjust(joinPublicGame->rect().center());
    joinPublicGame->setPos(center - joinPublicAdjust + QPointF(startLocalGame->rect().width() + 20, 0));

    // sub center
    changeNameButton->setRect(0, 0, scene->width() / 7, scene->height() / 7);
    QPointF changeNameAdjust(changeNameButton->rect().center());
    changeNameButton->setPos(center - changeNameAdjust + QPointF(0, startLocalGame->rect().height() + 20));

    server = new ChatServer();
    QObject::connect(server, &ChatServer::serverFull, this, &Game::sendServerFull);

    client = new ChatClient();
    QObject::connect(client, &ChatClient::error, this, &Game::error);
    QObject::connect(client, &ChatClient::dataReceived, this, &Game::receiveData);
    QObject::connect(client, &ChatClient::disconnected, this, &Game::disconnectedFromServer);
    QObject::connect(client, &ChatClient::connected, this, &Game::connectedToServer);

    newestEnemy = NULL;
    newestBullet = NULL;
}

void Game::mainFunction()
{
    scene()->advance();

    moveGun();

    cleanUpScene();
    bulletImpact();
    enemyImpact();

    if (justResumed) {
        fpsStopwatch->restart();
        justResumed = false;
    } else {
        frameTimes.append(fpsStopwatch->restart());
    }
}

void Game::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->drawPixmap(scene()->sceneRect().toRect(), background);
}

void Game::drawForeground(QPainter *painter, const QRectF &)
{
    QPainterPath path;
    path.addRect(scene()->sceneRect());
    path.addRect(QRectF(-300, -300, scene()->sceneRect().width() + 600, scene()->sceneRect().height() + 600));
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->drawPath(path);
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (!gameStarted) return;

    if (player->getDead()) return;

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
        nuke();
    } else if (key == Qt::Key_Space) {
        pauseButton->mousePress(QPointF(pauseButton->sceneBoundingRect().center()));
        pauseButton->mouseRelease();
    } else if (key == Qt::Key_E) {
        autoFire = !autoFire;
        if (autoFire) {
            shotsTimer->start();
        } else {
            shotsTimer->stop();
        }
    }
    if (mode == Multiplayer)
        sendPlayerDirection();
}

void Game::keyReleaseEvent(QKeyEvent *event)
{
    if (!gameStarted) return;

    if (player->getDead()) return;

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
    if (mode == Multiplayer)
        sendPlayerDirection();
}

void Game::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

void Game::mouseMoveEvent(QMouseEvent *event)
{
    QPointF point(mapToScene(event->pos()));

    if (!gameStarted) {
        for (auto button: buttons) {
            if (button->mouseMove(point)) {
                setCursor(QCursor(Qt::PointingHandCursor));
                return;
            }
        }
        setCursor(QCursor(Qt::ArrowCursor));
        return;
    }

    if (pauseButton->mouseMove(point))
        setCursor(QCursor(Qt::PointingHandCursor));
    else
        setCursor(QCursor(Qt::ArrowCursor));

    mouseTip = point;

    if (gamePaused || pauseButton->getPressed()) return;

    if (player->getDead()) return;

    moveGun();
}

void Game::mousePressEvent(QMouseEvent *event)
{
    QPointF point(mapToScene(event->pos()));

    if (!gameStarted) {
        for (auto button: buttons) {
            if (button->mousePress(point)) {
                return;
            }
        }
        return;
    }

    pauseButton->mousePress(point);

    if (gamePaused || pauseButton->getPressed()) return;

    if (player->getDead()) return;

    mouseMoveEvent(event);

    if (!autoFire) {
        shoot();
        delayTimer->start();
    }

}

void Game::mouseReleaseEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        for (auto button: buttons) {
            button->mouseRelease();
        }
        return;
    }

    pauseButton->mouseRelease();

    if (gamePaused || pauseButton->getPressed()) return;

    if (player->getDead()) return;

    mouseMoveEvent(event);

    if (!autoFire) {
        shotsTimer->stop();
        delayTimer->stop();
    }
}

void Game::resizeEvent(QResizeEvent *)
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

// same function, but without margin
void Game::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
    if (!scene() || rect.isNull())
            return;
    auto unity = transform().mapRect(QRectF(0, 0, 1, 1));
    if (unity.isEmpty())
        return;
    scale(1/unity.width(), 1/unity.height());
    auto viewRect = viewport()->rect();
    if (viewRect.isEmpty())
        return;
    auto sceneRect = transform().mapRect(rect);
    if (sceneRect.isEmpty())
        return;
    qreal xratio = viewRect.width() / sceneRect.width();
    qreal yratio = viewRect.height() / sceneRect.height();

    // Respect the aspect ratio mode.
    switch (aspectRatioMode) {
    case Qt::KeepAspectRatio:
        xratio = yratio = qMin(xratio, yratio);
        break;
    case Qt::KeepAspectRatioByExpanding:
        xratio = yratio = qMax(xratio, yratio);
        break;
    case Qt::IgnoreAspectRatio:
        break;
    }
    scale(xratio, yratio);
    centerOn(rect.center());
}

void Game::shoot()
{
    qreal angleRadian(qDegreesToRadians(player->getMouseAngle()));
    Bullet *bullet = new Bullet(gunTip, angleRadian);
    bullet->setIsPrimaryBullet(true);

    if (mode == Game::Multiplayer)
        sendNewBulletData(gunTip, angleRadian);

    bullets.append(bullet);
    newestBullet = bullet;
    scene()->addItem(bullet);
}

void Game::startFullAuto()
{
    shotsTimer->start();
}

void Game::startHealthRegen()
{
    regenTimer->start();
}

void Game::healthRegen()
{
    if (!player->getDead() && player->getHealth() < player->getStartHealth()) {
        player->setHealth(player->getHealth() + 1);
        playerHealthBar->update();
        player->update();
    }
}

void Game::cleanUpScene()
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

void Game::generateEnemy()
{
    QPointF startPoint(QRandomGenerator::system()->bounded(scene()->width()), 0);
    int size = enemySizes[QRandomGenerator::system()->bounded(7)];
    Enemy *enemy = new Enemy(startPoint, playerCenter, enemyVelo, size);

    if (mode == Game::Multiplayer)
        sendNewEnemyData(startPoint, playerCenter, enemyVelo, size);

    enemies.append(enemy);
    newestEnemy = enemy;
    scene()->addItem(enemy);
}

void Game::bulletImpact()
{
    for (int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        if (!enemy) continue;
        QRectF enemyRect(enemy->sceneBoundingRect());
        for (int j = 0; j < bullets.size(); j++) {
            Bullet *bullet = bullets.at(j);
            if (!bullet) continue;
            QRectF bulletRect(bullet->sceneBoundingRect());
            if (enemyRect.intersects(bulletRect)) {
                enemy->activateRegen();
                enemy->setHealth(enemy->getHealth() - bullet->getDamage());
                if (enemy->getHealth() <= 0) {
                    if (bullet->getIsPrimaryBullet())
                        setScore(player->getScore() + enemy->getStartHealth());
                    enemies.remove(i);
                    i--;
                    enemy->startExplosion();
                    break;
                }
                bullets.remove(j);
                j--;
                bullet->startExplosion();
            }
        }
    }
}

void Game::enemyImpact()
{
    QRectF playerRect(player->sceneBoundingRect());
    for (int i = 0; i < enemies.size(); i++) {
        Enemy *enemy = enemies.at(i);
        if (!enemy) continue;
        QRectF enemyRect(enemy->sceneBoundingRect());
        if (!player->getDead() && enemyRect.intersects(playerRect)) {
            enemy->setHealth(0);
            regenTimer->stop();
            regenDelayTimer->start();
            player->setHealth(player->getHealth() - enemy->getDamage());
            playerHealthBar->update();
            setScore(player->getScore() + enemy->getStartHealth());
            if (player->getHealth() <= 0) {
                if (mode == Game::Solo) {
                    gameEnd();
                    return;
                } else if (mode == Game::Multiplayer) {
                    makeEnemyTimer->stop();
                    delayTimer->stop();
                    shotsTimer->stop();
                    sendPlayerData();
                    if (status == Game::Host) {
                        bool allDead = true;
                        foreach (Player *otherPlayer, otherPlayersMap) {
                            if (!otherPlayer->getDead()) {
                                allDead = false;
                                break;
                            }
                        }
                        if (allDead) {
                            if (server->isListening()) {
                                server->stopServer();
                            }
                            return;
                        } else {
                            player->startExplosion();
                        }
                    } else if (status == Game::Guest) {
                        player->startExplosion();
                    }
                }
            }
            enemies.remove(i);
            i--;
            enemy->startExplosion();
            continue;
        }
        // for the visual effect of the enemy exploding
        if (mode == Game::Multiplayer) {
            foreach (Player *otherPlayer, otherPlayersMap) {
                if (otherPlayer->getDead()) continue;
                QRectF otherPlayerRect(otherPlayer->sceneBoundingRect());
                if (enemyRect.intersects(otherPlayerRect)) {
                    enemy->setHealth(0);
                    enemies.remove(i);
                    i--;
                    enemy->startExplosion();
                    break;
                }
            }
        }
    }
}

void Game::moveGun()
{
    playerCenter = player->pos() + QPointF(player->getSize() / 2, player->getSize() / 2);

    QLineF mouseLine(playerCenter, mouseTip);

    player->setMouseAngle(mouseLine.angle());
    // 60 refers to pixels away from player center
    gunTip = mouseLine.pointAt(60 / mouseLine.length());

    player->getGun()->rotate(player->getMouseAngle());
}

void Game::nuke()
{
    for (auto enemy: enemies) {
        enemy->startExplosion();
    }
    for (auto bullet: bullets) {
        bullet->startExplosion();
    }
    enemies.clear();
    bullets.clear();
}

void Game::setScene()
{
    QPointF center(scene()->sceneRect().center());

    if (mode == Game::Solo) {
        QPointF playerAdjust(player->getSize() / 2, player->getSize() / 2);
        player->setPos(center - playerAdjust);
    } else if (mode == Game::Multiplayer) {
        QPointF newPos(QRandomGenerator::system()->bounded(scene()->width() - player->getSize()),
                       center.y() - player->getSize() / 2);
        player->setPos(newPos);
    }

    mouseTip = QPointF(scene()->width() / 2, 0);
}

void Game::gameStart()
{
    setScore(0);
    level = 1;
    enemyVelo = 2;
    upHeld = downHeld = leftHeld = rightHeld = false;
    player->resetProperties();
    setScene();

    setCursor(QCursor(Qt::ArrowCursor));

    scene()->removeItem(title);
    scene()->removeItem(highScoreText);
    scene()->removeItem(startLocalGame);
    scene()->removeItem(startPublicGame);
    scene()->removeItem(joinPublicGame);
    scene()->removeItem(changeNameButton);

    scene()->addItem(fpsText);
    scene()->addItem(player);

    if (mode == Game::Multiplayer) {
        foreach (Player *otherPlayer, otherPlayersMap) {
            scene()->addItem(otherPlayer);
        }
        pauseButton->setEnabled(false);
    }
    if (mode == Game::Solo) {
        scene()->addItem(pauseButton);
        pauseButton->setEnabled(true);
    }

    scene()->addItem(playerHealthBar);
    scoreText->setVisible(true);

    gameStarted = true;

    mainTimer->start();
    if (mode == Game::Multiplayer) sendDataTimer->start();
    fpsTimer->start();
    makeEnemyTimer->start();
}

void Game::gameEnd()
{
    gameStarted = false;
    gamePaused = false;
    justResumed = false;
    autoFire = false;

    serverSize = 0;

    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();

    // clean up any enemies still animating
    auto s = scene()->items();
    for (auto item: s) {
        Enemy *enemy = dynamic_cast<Enemy *>(item);
        if (enemy != NULL) {
            enemy->deleteLater();
            continue;
        }
        Bullet *bullet = dynamic_cast<Bullet *>(item);
        if (bullet != NULL) {
            bullet->deleteLater();
        }
    }

    scene()->removeItem(player);

    if (mode == Game::Multiplayer) {
        qDeleteAll(otherPlayersMap);
        otherPlayersMap.clear();
    }
    if (mode == Game::Solo) {
        scene()->removeItem(pauseButton);
    }

    pauseButton->reset();
    startLocalGame->reset();
    startPublicGame->reset();
    joinPublicGame->reset();

    scene()->removeItem(playerHealthBar);
    scene()->removeItem(fpsText);

    scene()->addItem(title);
    scene()->addItem(highScoreText);
    scene()->addItem(startLocalGame);
    scene()->addItem(startPublicGame);
    scene()->addItem(joinPublicGame);
    scene()->addItem(changeNameButton);

    mainTimer->stop();
    sendDataTimer->stop();
    fpsTimer->stop();
    frameTimes.clear();
    shotsTimer->stop();
    delayTimer->stop();
    makeEnemyTimer->stop();
    regenTimer->stop();
    regenDelayTimer->stop();

    if (player->getScore() > highScore) {
        highScore = player->getScore();
        QSettings settings("BenMax Productions", "BenMaxGame");
        settings.setValue("highScore", highScore);
        highScoreText->setPlainText(QString::number(highScore));
        QRectF highScoreAdjust = highScoreText->boundingRect();
        highScoreText->setPos(scene()->width() - highScoreAdjust.width(), scene()->height() - highScoreAdjust.height());
    }
}

void Game::gamePause()
{
    if (!gameStarted) return;

    gamePaused = !gamePaused;
    if (gamePaused) {
        mainTimer->pause();
        if (mode == Game::Multiplayer) sendDataTimer->pause();
        fpsTimer->pause();

        if (autoFire) {
            shotsTimer->pause();
        } else {
            shotsTimer->stop();
            delayTimer->stop();
        }

        makeEnemyTimer->pause();

        regenTimer->pause();
        regenDelayTimer->pause();

        for (auto enemy: enemies)
            enemy->pause();
    } else {
        mainTimer->resume();
        if (mode == Game::Multiplayer) sendDataTimer->resume();
        fpsTimer->resume();
        justResumed = true;

        if (autoFire) {
            shotsTimer->resume();
        }

        makeEnemyTimer->resume();

        regenTimer->resume();
        regenDelayTimer->resume();

        for (auto enemy: enemies)
            enemy->resume();
    }
}

void Game::setScore(int newScore)
{
    player->setScore(newScore);
    scoreText->setPlainText(QString::number(newScore));
    if (newScore >= level * 500) {
        level++;
        enemyVelo++;
    }

}

void Game::setFps()
{
    float averageTime = 0;
    for (auto time: frameTimes)
        averageTime += time;
    averageTime /= frameTimes.size();
    fpsText->setPlainText(QString::number((int)(1000 / averageTime)) + " FPS");
    frameTimes.clear();
}

void Game::resetHS()
{
    highScore = 0;
}

void Game::toggleStartServer()
{
    if (!server->isListening()) {
        if (!server->listen(QHostAddress::Any, 1967)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"));
            server->stopServer();
            return;
        }
        bool success;
        int players = QInputDialog::getInt(this, "Server Size", "Players:", 2, 2, 10, 1, &success);
        if (success) {
            server->setServerSize(players);
            startPublicGame->setNameAlt();
        } else {
            server->stopServer();
            startPublicGame->reset();
            return;
        }

        startLocalGame->setEnabled(false);
        joinPublicGame->setEnabled(false);

        status = Game::Host;

        client->connectToServer(QHostAddress::LocalHost, 1967);

    } else {
        server->stopServer();
        startLocalGame->setEnabled(true);
        joinPublicGame->setEnabled(true);
    }
}

void Game::attemptConnection()
{
    if (client->clientSocket()->state() != QAbstractSocket::ConnectedState) {
        // We ask the user for the address of the server, we use 127.0.0.1 (aka localhost) as default
        const QString hostAddress = QInputDialog::getText(
            this
            , tr("Choose Server")
            , tr("Server Address:")
            , QLineEdit::Normal
            , QStringLiteral("127.0.0.1")
        );
        if (hostAddress.isEmpty())
            return; // the user pressed cancel or typed nothing

        status = Game::Guest;
        // tell the client to connect to the host using the port 1967
        client->connectToServer(QHostAddress(hostAddress), 1967);

    } else {
        client->disconnectFromHost();
    }
}

void Game::connectedToServer()
{
    if (status == Game::Guest) {
        joinPublicGame->setNameAlt();
        startLocalGame->setEnabled(false);
        startPublicGame->setEnabled(false);
    }
}

// need to handle case of people leaving during game
void Game::disconnectedFromServer()
{
    // if the client loses connection to the server
    // comunicate the event to the user via a message box
//    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));

    joinPublicGame->reset();
    startLocalGame->setEnabled(true);
    startPublicGame->setEnabled(true);

    if (gameStarted)
        gameEnd();
}

void Game::changeName()
{
//    QLineEdit *lineEdit = inputDialog->findChild<QLineEdit*>();
    bool success;
    const QString newName = QInputDialog::getText(
        this
        , tr("Choose Name")
        , tr("Enter Name:")
        , QLineEdit::Normal
        , player->getName()
        , &success
    );
    if (!success) return;
    player->setName(newName);
    QSettings settings("BenMax Productions", "BenMaxGame");
    settings.setValue("name", newName);
}

void Game::error(QAbstractSocket::SocketError socketError)
{
    // show a message to the user that informs of what kind of error occurred
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return; // handled by disconnectedFromServer
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }
}

void Game::sendPlayerData()
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::PlayerData);
    data.setPlayerId(player->getId());
    data.setPlayerPos(player->pos());
    data.setPlayerMouseAngle(player->getMouseAngle());
    data.setPlayerHealth(player->getHealth());
    data.setPlayerScore(player->getScore());
    clientStream << data;
}

void Game::sendPlayerDirection()
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::PlayerDirection);
    data.setPlayerId(player->getId());
    data.setLeft(player->getLeft());
    data.setRight(player->getRight());
    data.setUp(player->getUp());
    data.setDown(player->getDown());
    clientStream << data;
}

void Game::sendNewPlayerData()
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::NewPlayer);
    data.setPlayerId(player->getId());
    data.setPlayerName(player->getName());
    clientStream << data;
}

void Game::sendNewBulletData(QPointF gunTip, qreal angle)
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::NewBullet);
    data.setBulletGunTip(gunTip);
    data.setBulletAngle(angle);
    clientStream << data;
}

void Game::sendNewEnemyData(QPointF startPoint, QPointF playerCenter, int velo, int size)
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::NewEnemy);
    data.setEnemyStartPoint(startPoint);
    data.setEnemyPlayerCenter(playerCenter);
    data.setEnemyVelo(velo);
    data.setEnemySize(size);
    clientStream << data;
}

void Game::sendServerFull(int totalPlayers)
{
    serverSize = totalPlayers;

    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    // tell each player to broadcast its player
    Data data(Data::ServerFull);
    data.setServerSize(totalPlayers);
    clientStream << data;

    // broadcast host's player
    sendNewPlayerData();
}

void Game::sendGameOver()
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::GameOver);
    clientStream << data;
}

void Game::receiveData(Data data)
{
    switch (data.getType()) {
    case Data::PlayerData: {
        if (!gameStarted) break;
        Player *otherPlayer = otherPlayersMap[data.getPlayerId()];
        otherPlayer->setPos(data.getPlayerPos());
        otherPlayer->getGun()->rotate(data.getPlayerMouseAngle());
        otherPlayer->setScore(data.getPlayerScore());
        otherPlayer->setHealth(data.getPlayerHealth());
        if (otherPlayer->getHealth() <= 0) {
            if (status == Game::Host) {
                bool allDead = true;
                if (!player->getDead()) allDead = false;
                foreach (Player *p, otherPlayersMap) {
                    if (p == otherPlayer) continue;
                    if (!p->getDead()) {
                        allDead = false;
                        break;
                    }
                }
                if (allDead) {
                    if (server->isListening()) {
                        server->stopServer();
                    }
                    return;
                } else {
                    otherPlayer->startExplosion();
                }
            } else if (status == Game::Guest) {
                otherPlayer->startExplosion();
            }
        }
        break;
    }
    case Data::PlayerDirection: {
        if (!gameStarted) break;
        Player *otherPlayer = otherPlayersMap[data.getPlayerId()];
        otherPlayer->setLeft(data.getLeft());
        otherPlayer->setRight(data.getRight());
        otherPlayer->setUp(data.getUp());
        otherPlayer->setDown(data.getDown());
        break;
    }
    case Data::NewPlayer: {
        QUuid id = data.getPlayerId();
        QString name = data.getPlayerName();
        if (!otherPlayersMap.contains(id) && player->getId() != id) {
            Player *player = new Player(id, name);
            otherPlayersMap[id] = player;
        }
        if (serverSize == otherPlayersMap.size() + 1) {
            startServerGame();
        }
        break;
    }
    case Data::NewBullet: {
        if (!gameStarted) break;
        Bullet *bullet = new Bullet(data.getBulletGunTip(), data.getBulletAngle());
        bullet->setIsPrimaryBullet(false);
        bullets.append(bullet);
        scene()->addItem(bullet);
        break;
    }
    case Data::NewEnemy: {
        if (!gameStarted) break;
        Enemy *enemy = new Enemy(data.getEnemyStartPoint(), data.getEnemyPlayerCenter(),
                                 data.getEnemyVelo(), data.getEnemySize());
        enemies.append(enemy);
        scene()->addItem(enemy);
        break;
    }
    case Data::ServerFull:
        serverSize = data.getServerSize();
        sendNewPlayerData();
        break;
    case Data::GameOver:
        if (!gameStarted) break;
        gameEnd();
        break;
    }
}

void Game::startSoloGame()
{
    mode = Game::Solo;
    gameStart();
}

void Game::startServerGame()
{
    mode = Game::Multiplayer;
    gameStart();
}


