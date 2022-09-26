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

Game::Game(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    gameStarted = false;
    gamePaused = false;

    mainTimer = new QTimer();
    QObject::connect(mainTimer, &QTimer::timeout, this, &Game::mainFunction);
    mainTimer->setInterval(10);

    shotsTimer = new QTimer();
    QObject::connect(shotsTimer, &QTimer::timeout, this, &Game::shoot);
//    shotsTimer->setInterval(50);
    shotsTimer->setInterval(150);

    delayTimer = new QTimer();
    delayTimer->setSingleShot(true);
    QObject::connect(delayTimer, &QTimer::timeout, this, &Game::startFullAuto);
    delayTimer->setInterval(250);

    makeEnemyTimer = new QTimer();
    QObject::connect(makeEnemyTimer, &QTimer::timeout, this, &Game::generateEnemy);
//    makeEnemyTimer->setInterval(200);
    makeEnemyTimer->setInterval(1000);

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
    QObject::connect(pauseButton, &Button::clicked, this, &Game::gamePause);
    pauseButton->setRect(0, 0, 100, 100);
    pauseButton->setFontDivisor(2);
    pauseButton->setZValue(1);
//    pauseButton->setIcon(style()->standardPixmap(QStyle::SP_MediaPause));

//    server = new ChatServer();
//    serverButton = new Button("Start Server");
//    serverButton->setRect(0, 0, 50, 50);
//    serverButton->setFontDivisor(6);
//    scene->addItem(serverButton);
//    QObject::connect(serverButton, &Button::clicked, this, &Game::toggleStartServer);

//    chatWindow = new ChatWindow();
//    QObject::connect(chatWindow, &ChatWindow::readyToStart, this, &Game::gameStart);
//    QObject::connect(chatWindow, &ChatWindow::playerJoined, this, &Game::addPlayer);
//    QObject::connect(playButton, &Button::clicked, chatWindow, &ChatWindow::attemptConnection);

//    QObject::connect(player, &Player::moved, chatWindow, &ChatWindow::sendMessage);
//    QObject::connect(chatWindow, &ChatWindow::playerMoved, this, &Game::moveOtherPlayer);

    startLocalGame = new Button("Play\nSolo");
    scene->addItem(startLocalGame);
    QObject::connect(startLocalGame, &Button::clicked, this, &Game::gameStart);

    startPublicGame = new Button("Start\nPublic\nGame");
    scene->addItem(startPublicGame);
    startPublicGame->setFontDivisor(6);
    QObject::connect(startPublicGame, &Button::clicked, this, &Game::toggleStartServer);

    joinPublicGame = new Button("Join\nPublic\nGame");
    scene->addItem(joinPublicGame);
    joinPublicGame->setFontDivisor(6);
    QObject::connect(joinPublicGame, &Button::clicked, this, &Game::attemptConnection);

    server = NULL;
    client = NULL;

    otherPlayer = NULL;
    otherPlayerHealthBar = NULL;
}

void Game::mainFunction()
{
    scene()->advance();
    moveGun();

    sendData();

    cleanUpScene();
    bulletImpact();
    enemyImpact();
}

void Game::keyPressEvent(QKeyEvent *event)
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

void Game::keyReleaseEvent(QKeyEvent *event)
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

void Game::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

void Game::mouseMoveEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        playButton->mouseMove(event->position());
//        serverButton->mouseMove(event->position());
        startPublicGame->mouseMove(event->position());
        startLocalGame->mouseMove(event->position());
        joinPublicGame->mouseMove(event->position());
        return;
    }

    pauseButton->mouseMove(event->position());

    if (gamePaused || pauseButton->getPressed()) return;

    mouseTip.setX(event->position().x());
    mouseTip.setY(event->position().y());

    moveGun();

    sendData();
}

void Game::mousePressEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        playButton->mousePress(event->position());
//        serverButton->mousePress(event->position());
        startPublicGame->mousePress(event->position());
        startLocalGame->mousePress(event->position());
        joinPublicGame->mousePress(event->position());
        return;
    }

    pauseButton->mousePress(event->position());

    if (gamePaused || pauseButton->getPressed()) return;

    mouseMoveEvent(event);

    shoot();
    delayTimer->start();
}

void Game::mouseReleaseEvent(QMouseEvent *event)
{
    if (!gameStarted) {
        playButton->mouseRelease();
//        serverButton->mouseRelease();
        startPublicGame->mouseRelease();
        startLocalGame->mouseRelease();
        joinPublicGame->mouseRelease();
        return;
    }

    pauseButton->mouseRelease();

    if (gamePaused || pauseButton->getPressed()) return;

    mouseMoveEvent(event);

    shotsTimer->stop();
    delayTimer->stop();
}

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

    playButton->setRect(0, 0, width() / 7, height() / 7);
    QPointF center(newSceneRect.center());
    QPointF playButtonAdjust(playButton->rect().center());
    playButton->setPos(center - playButtonAdjust);

    pauseButton->setRect(0, 0, width() / 20, width() / 20);
    QRectF pauseButtonAdjust = pauseButton->boundingRect();
    pauseButton->setPos(width() - pauseButtonAdjust.width(), height() - pauseButtonAdjust.height());

//    serverButton->setRect(0, 0, width() / 8, height() / 9);
//    QRectF serverButtonAdjust = serverButton->boundingRect();
//    serverButton->setPos(width() - serverButtonAdjust.width(), 0);



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
    qreal angleRadian(qDegreesToRadians(mouseAngle));
    Bullet *bullet = new Bullet(gunTip, angleRadian);
    bullets.append(bullet);
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
    Enemy *enemy = new Enemy(startPoint, playerCenter, enemyVelo);
    enemies.append(enemy);
    scene()->addItem(enemy);
}

void Game::bulletImpact()
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

void Game::enemyImpact()
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

void Game::moveGun()
{
    playerCenter.setX(player->x() + (player->getSize() / 2));
    playerCenter.setY(player->y() + (player->getSize() / 2));

    QLineF mouseLine(playerCenter, mouseTip);

    mouseAngle = mouseLine.angle();
    gunTip = mouseLine.pointAt(60 / mouseLine.length());

    player->getGun()->rotate(mouseAngle);
}

void Game::setScene()
{
    QRectF newSceneRect(0, 0, width(), height());
    QPointF center(newSceneRect.center());
    QPointF playerAdjust(player->getSize() / 2, player->getSize() / 2);
    player->setPos(center - playerAdjust);
    playerHealthBar->setPos(newSceneRect.width() / 2 - 100, newSceneRect.height() - 50);
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
    scene()->removeItem(playButton);
    scene()->removeItem(highScoreText);
//    scene()->removeItem(serverButton);
    scene()->removeItem(startLocalGame);
    scene()->removeItem(startPublicGame);
    scene()->removeItem(joinPublicGame);

    scene()->addItem(player);
    scene()->addItem(playerHealthBar);

    if (otherPlayer) {
        scene()->addItem(otherPlayer);
        scene()->addItem(otherPlayerHealthBar);
    }


    scene()->addItem(pauseButton);
    scoreText->setVisible(true);

    gameStarted = true;

    mainTimer->start();
    makeEnemyTimer->start();
}

void Game::gameEnd()
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
//    scene()->addItem(serverButton);
    scene()->addItem(startLocalGame);
    scene()->addItem(startPublicGame);
    scene()->addItem(joinPublicGame);

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

//    chatWindow->endGame();
}

void Game::gamePause()
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

void Game::setScore(int newScore)
{
    score = newScore;
    scoreText->setPlainText(QString::number(score));
    if (score >= level + 500) {
        level += 500;
        enemyVelo++;
    }
}

void Game::toggleStartServer()
{
    if (server == NULL) {
        server = new ChatServer();
        if (!server->listen(QHostAddress::Any, 1967)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"));
            return;
        }
        startPublicGame->setButtonName("Stop\nPublic\nGame");
        startLocalGame->setEnabled(false);
        joinPublicGame->setEnabled(false);

        client = new ChatClient();
        QObject::connect(client, &ChatClient::error, this, &Game::error);
        QObject::connect(client, &ChatClient::dataReceived, this, &Game::receiveData);

        client->connectToServer(QHostAddress("127.0.0.1"), 1967);

        // now must wait for a connection
        // upon connection, make second player and start game
        // secondary game will begin transmitting data to the server
        QObject::connect(server, &ChatServer::playerJoined, this, &Game::addPlayer);
    } else {
        server->stopServer();
        delete server;
        server = NULL;
        startPublicGame->setButtonName("Start\nPublic\nGame");
        startLocalGame->setEnabled(true);
        joinPublicGame->setEnabled(true);
    }
}

void Game::attemptConnection()
{
    // We ask the user for the address of the server, we use 127.0.0.1 (aka localhost) as default
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Chose Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral("127.0.0.1")
    );
    if (hostAddress.isEmpty())
        return; // the user pressed cancel or typed nothing


    // tell the client to connect to the host using the port 1967
    client = new ChatClient();
    QObject::connect(client, &ChatClient::error, this, &Game::error);
    QObject::connect(client, &ChatClient::connected, this, &Game::addPlayer);
    QObject::connect(client, &ChatClient::dataReceived, this, &Game::receiveData);

    client->connectToServer(QHostAddress(hostAddress), 1967);
    // host decides when game starts
    // name doesn't matter for now
}

void Game::connectedToServer()
{
    // prepare environment for being a secondary instance of the game
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

void Game::sendData()
{
    if (!client) return;
    QDataStream clientStream(client->clientSocket());
    clientStream.setVersion(QDataStream::Qt_5_7);
    // will send ID first
    Data gameData(player->pos(), mouseAngle, player->getHealth(), score);
    gameData.operator<<(clientStream);
}

void Game::receiveData(Data data)
{
    otherPlayer->setPos(data.getPlayerPos());
    otherPlayer->getGun()->rotate(data.getMouseAngle());
    otherPlayer->setHealth(data.getHealth());
}


// maybe have server window be a QDockWindow
//void Game::toggleStartServer()
//{
//    if (server->isListening()) {
//        server->stopServer();
//        serverButton->setButtonName("Start Server");
//    } else {
//        if (!server->listen(QHostAddress::Any, 1967)) {
//            QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"));
//            return;
//        }
//        serverButton->setButtonName("Stop Server");
//    }
//}

void Game::addPlayer()
{
    Player *newPlayer = new Player();
    HealthBar *newPlayerHealthBar = new HealthBar(newPlayer);
    newPlayerHealthBar->setZValue(1);
//    otherPlayers.append(newPlayer);
    otherPlayer = newPlayer;
    otherPlayerHealthBar = newPlayerHealthBar;

    QRectF newSceneRect(0, 0, width(), height());
    QPointF center(newSceneRect.center());
    QPointF playerAdjust(newPlayer->getSize() / 2, newPlayer->getSize() / 2);
    newPlayer->setPos(center - playerAdjust);
    newPlayerHealthBar->setPos(newSceneRect.width() / 2 - 100, newSceneRect.height() - 100);

    gameStart();
}

void Game::moveOtherPlayer(QPointF pos)
{
    otherPlayer->setPos(pos);
}


