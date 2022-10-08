#include "game.h"
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
#include "data.h"
#include "serverworker.h"

int enemySizes[] = {30, 50, 70, 90, 110, 130, 150};

Game::Game(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    gameStarted = false;
    gamePaused = false;

    serverSize = 0;

    fps = 0;

    mainTimer = new QTimer();
    QObject::connect(mainTimer, &QTimer::timeout, this, &Game::mainFunction);
    // calculations happen 100 times per second
    mainTimer->setInterval(10);

    shotsTimer = new QTimer();
    QObject::connect(shotsTimer, &QTimer::timeout, this, &Game::shoot);
    shotsTimer->setInterval(50);
//    shotsTimer->setInterval(250);

    delayTimer = new QTimer();
    delayTimer->setSingleShot(true);
    QObject::connect(delayTimer, &QTimer::timeout, this, &Game::startFullAuto);
    delayTimer->setInterval(250);

    makeEnemyTimer = new QTimer();
    QObject::connect(makeEnemyTimer, &QTimer::timeout, this, &Game::generateEnemy);
    makeEnemyTimer->setInterval(200);
//    makeEnemyTimer->setInterval(1500);

    fpsTimer = new QTimer();
    QObject::connect(fpsTimer, &QTimer::timeout, this, &Game::setFps);
    fpsTimer->setInterval(100);

    fpsText = new QGraphicsTextItem("0");
    fpsText->setDefaultTextColor(Qt::white);
    scene->addItem(fpsText);
    fpsText->setZValue(1);


    player = new Player(QUuid::createUuid());
//    playerHealthBar = new HealthBar(player);
//    playerHealthBar->setZValue(1);

    background = QPixmap(":/images/space3.jpg");

    title = new QGraphicsTextItem("BenMaxGame");
    title->setDefaultTextColor(Qt::white);
    scene->addItem(title);

    scoreText = new QGraphicsTextItem(QString::number(player->getScore()));
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

    pauseButton = new Button("II");
    QObject::connect(pauseButton, &Button::clicked, this, &Game::gamePause);
    pauseButton->setRect(0, 0, 100, 100);
    pauseButton->setFontDivisor(2);
    pauseButton->setZValue(1);
//    pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPause));

    startLocalGame = new Button("Play\nSolo");
    scene->addItem(startLocalGame);
    QObject::connect(startLocalGame, &Button::clicked, this, &Game::startSoloGame);

    startPublicGame = new Button("Start\nPublic\nGame");
    scene->addItem(startPublicGame);
    startPublicGame->setFontDivisor(6);
    QObject::connect(startPublicGame, &Button::clicked, this, &Game::toggleStartServer);

    joinPublicGame = new Button("Join\nPublic\nGame");
    scene->addItem(joinPublicGame);
    joinPublicGame->setFontDivisor(6);
    QObject::connect(joinPublicGame, &Button::clicked, this, &Game::attemptConnection);

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

    if (!player->getDead()) {
        moveGun();
        if (mode == Game::Multiplayer)
            sendPlayerData();
    }

    cleanUpScene();
    bulletImpact();
    enemyImpact();

    fps++;
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
        qDeleteAll(enemies);
        enemies.clear();
    } else if (key == Qt::Key_Space) {
        pauseButton->mousePress(QPointF(pauseButton->sceneBoundingRect().center()));
        pauseButton->mouseRelease();
    }
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
}

void Game::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

void Game::mouseMoveEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        startPublicGame->mouseMove(event->position());
        startLocalGame->mouseMove(event->position());
        joinPublicGame->mouseMove(event->position());
        return;
    }

    pauseButton->mouseMove(event->position());

    if (gamePaused || pauseButton->getPressed()) return;

    if (player->getDead()) return;

    mouseTip.setX(event->position().x());
    mouseTip.setY(event->position().y());

    moveGun();

    if (mode == Game::Multiplayer)
        sendPlayerData();
}

void Game::mousePressEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        startPublicGame->mousePress(event->position());
        startLocalGame->mousePress(event->position());
        joinPublicGame->mousePress(event->position());
        return;
    }

    pauseButton->mousePress(event->position());

    if (gamePaused || pauseButton->getPressed()) return;

    if (player->getDead()) return;

    mouseMoveEvent(event);

    shoot();
    delayTimer->start();
}

void Game::mouseReleaseEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        startPublicGame->mouseRelease();
        startLocalGame->mouseRelease();
        joinPublicGame->mouseRelease();
        return;
    }

    pauseButton->mouseRelease();

    if (gamePaused || pauseButton->getPressed()) return;

    if (player->getDead()) return;

    mouseMoveEvent(event);

    shotsTimer->stop();
    delayTimer->stop();
}

// maybe text and background should fit to scale but game items should be adjusted
// maybe position of game items should be adjusted but they should keep their size
void Game::resizeEvent(QResizeEvent *event)
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

    fpsText->setFont(QFont("Arial", width() / 50, QFont::Bold));
    fpsText->setPos(0, 0);

    pauseButton->setRect(0, 0, width() / 20, width() / 20);
    QRectF pauseButtonAdjust = pauseButton->boundingRect();
    pauseButton->setPos(width() - pauseButtonAdjust.width(), height() - pauseButtonAdjust.height());

    QPointF center(newSceneRect.center());

    // center
    startPublicGame->setRect(0, 0, width() / 7, height() / 7);
    QPointF publicGameAdjust(startPublicGame->rect().center());
    startPublicGame->setPos(center - publicGameAdjust);

    // left of center
    startLocalGame->setRect(0, 0, width() / 7, height() / 7);
    QPointF localGameAdjust(startLocalGame->rect().center());
    startLocalGame->setPos(center - localGameAdjust - QPointF(startPublicGame->rect().width() + 20, 0));

    // right of center
    joinPublicGame->setRect(0, 0, width() / 7, height() / 7);
    QPointF joinPublicAdjust(joinPublicGame->rect().center());
    joinPublicGame->setPos(center - joinPublicAdjust + QPointF(startPublicGame->rect().width() + 20, 0));


    QGraphicsView::resizeEvent(event);
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
            player->setHealth(player->getHealth() - enemy->getDamage());
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
//            playerHealthBar->update();
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
    playerCenter.setX(player->x() + (player->getSize() / 2));
    playerCenter.setY(player->y() + (player->getSize() / 2));

    QLineF mouseLine(playerCenter, mouseTip);

    player->setMouseAngle(mouseLine.angle());
    gunTip = mouseLine.pointAt(60 / mouseLine.length());

    player->getGun()->rotate(player->getMouseAngle());
}

void Game::setScene()
{
    QRectF newSceneRect(0, 0, width(), height());
    QPointF center(newSceneRect.center());

    if (mode == Game::Solo) {
        QPointF playerAdjust(player->getSize() / 2, player->getSize() / 2);
        player->setPos(center - playerAdjust);
    } else if (mode == Game::Multiplayer) {
        QPointF newPos(QRandomGenerator::system()->bounded(scene()->width() - player->getSize()), center.y() - player->getSize() / 2);
        player->setPos(newPos);
    }

//    playerHealthBar->setPos(newSceneRect.width() / 2 - 100, newSceneRect.height() - 50);
    mouseTip = QPointF(width() / 2, 0);
}

void Game::gameStart()
{
    setScore(0);
    level = 0;
    enemyVelo = 2;
    upHeld = downHeld = leftHeld = rightHeld = false;
    player->resetProperties();
    setScene();

    scene()->removeItem(title);
    scene()->removeItem(highScoreText);
    scene()->removeItem(startLocalGame);
    scene()->removeItem(startPublicGame);
    scene()->removeItem(joinPublicGame);

    scene()->addItem(player);


    if (mode == Game::Multiplayer) {
        foreach (Player *otherPlayer, otherPlayersMap) {
            scene()->addItem(otherPlayer);
        }
        pauseButton->setEnabled(false);
        pauseButton->setVisible(false);
    }

//    scene()->addItem(playerHealthBar);

    scene()->addItem(pauseButton);
    scoreText->setVisible(true);

    gameStarted = true;

    mainTimer->start();
    fpsTimer->start();
    makeEnemyTimer->start();
}

void Game::gameEnd()
{
    gameStarted = false;
    gamePaused = false;

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

        serverSize = 0;

        startPublicGame->setButtonName("Start\nPublic\nGame");
        joinPublicGame->setButtonName("Join\nPublic\nGame");

        startLocalGame->setEnabled(true);
        startPublicGame->setEnabled(true);
        joinPublicGame->setEnabled(true);

        pauseButton->setEnabled(true);
        pauseButton->setVisible(true);
    }

//    scene()->removeItem(playerHealthBar);

    scene()->removeItem(pauseButton);

    scene()->addItem(title);
    scene()->addItem(highScoreText);
    scene()->addItem(startLocalGame);
    scene()->addItem(startPublicGame);
    scene()->addItem(joinPublicGame);



    mainTimer->stop();
    fpsTimer->stop();
    fps = 0;
    shotsTimer->stop();
    delayTimer->stop();
    makeEnemyTimer->stop();

    if (player->getScore() > highScore) {
        highScore = player->getScore();
        QSettings settings("BenMax Productions", "BenMaxGame");
        settings.setValue("highScore", highScore);
        highScoreText->setPlainText(QString::number(highScore));
        QRectF highScoreAdjust = highScoreText->boundingRect();
        highScoreText->setPos(width() - highScoreAdjust.width(), height() - highScoreAdjust.height());
    }
}

void Game::gamePause()
{
    gamePaused = !gamePaused;
    if (gamePaused) {
        mainTimer->stop();
        fpsTimer->stop();
        fps = 0;
        shotsTimer->stop();
        delayTimer->stop();
        makeEnemyTimer->stop();
        pauseButton->setButtonName(">");
//        pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPlay));
    } else {
        mainTimer->start();
        fpsTimer->start();
        makeEnemyTimer->start();
        pauseButton->setButtonName("II");
//        pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPause));
    }
}

void Game::setScore(int newScore)
{
    player->setScore(newScore);
    scoreText->setPlainText(QString::number(player->getScore()));
    if (player->getScore() >= level + 500) {
        level += 500;
        enemyVelo++;
    }
}

void Game::setFps()
{
    float scale = 1000 / (float)(fpsTimer->interval());
    fpsText->setPlainText(QString::number((int)((float)fps * scale)));
    fps = 0;
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
        } else {
            server->stopServer();
            return;
        }

        startPublicGame->setButtonName("Stop\nPublic\nGame");
        startLocalGame->setEnabled(false);
        joinPublicGame->setEnabled(false);

        status = Game::Host;

        client->connectToServer(QHostAddress::LocalHost, 1967);

    } else {
        server->stopServer();

        startPublicGame->setButtonName("Start\nPublic\nGame");
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

        // tell the client to connect to the host using the port 1967

        status = Game::Guest;

        client->connectToServer(QHostAddress(hostAddress), 1967);

    } else {
        client->disconnectFromHost();
    }
}

void Game::connectedToServer()
{
    if (status == Game::Guest) {
        joinPublicGame->setButtonName("Exit\nPublic\nGame");
        startLocalGame->setEnabled(false);
        startPublicGame->setEnabled(false);
    }
}

void Game::disconnectedFromServer()
{
    // if the client loses connection to the server
    // comunicate the event to the user via a message box
//    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));

    joinPublicGame->setButtonName("Join\nPublic\nGame");
    startLocalGame->setEnabled(true);
    startPublicGame->setEnabled(true);

    if (gameStarted)
        gameEnd();
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

    Data data(Data::PlayerMove);
    data.setPlayerId(player->getId());
    data.setPlayerPos(player->pos());
    data.setPlayerMouseAngle(player->getMouseAngle());
    data.setPlayerHealth(player->getHealth());
    data.setPlayerScore(player->getScore());
    clientStream << data;
}

void Game::sendNewPlayerData()
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());

    Data data(Data::NewPlayer);
    data.setPlayerId(player->getId());
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
    case Data::PlayerMove: {
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
    case Data::NewPlayer: {
        QUuid id = data.getPlayerId();
        if (!otherPlayersMap.contains(id) && player->getId() != id) {
            Player *player = new Player(id);
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


