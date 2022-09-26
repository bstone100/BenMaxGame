#ifndef GAME_H
#define GAME_H

#include "button.h"
#include "chatclient.h"
#include "chatserver.h"
#include "chatwindow.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include <QGraphicsView>

class Game : public QGraphicsView
{
    Q_OBJECT
public:
    Game(QGraphicsScene *scene, QWidget *parent = nullptr);

    void mainFunction();

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void shoot();
    void startFullAuto();
    void cleanUpScene();
    void generateEnemy();
    void bulletImpact();
    void enemyImpact();
    void moveGun();

    void setScene();
    void gameStart();
    void gameEnd();
    void gamePause();

    void setScore(int newScore);

//    void toggleStartServer();
    void addPlayer();

    void moveOtherPlayer(QPointF pos);

    void toggleStartServer();
    void attemptConnection();
    void connectedToServer();
    void error(QAbstractSocket::SocketError socketError);
    void sendData();
    void receiveData(Data data);


private:
    bool upHeld, downHeld, leftHeld, rightHeld;

    QPointF mouseTip, playerCenter, gunTip;

    qreal mouseAngle;

    Player *player;
    HealthBar *playerHealthBar;
    QVector<Bullet *> bullets;
    QVector<Enemy *> enemies;
//    QVector<Player *> otherPlayers;
    Player *otherPlayer;
    HealthBar *otherPlayerHealthBar;

    QTimer *mainTimer;
    QTimer *shotsTimer;
    QTimer *delayTimer;
    QTimer *cleanUpTimer;
    QTimer *makeEnemyTimer;
    QTimer *bulletImpactTimer;

    QGraphicsLineItem *xAxis;
    QGraphicsLineItem *yAxis;
    QGraphicsRectItem *box;

    QPixmap background;

    QGraphicsTextItem *title;
    QGraphicsTextItem *scoreText;
    QGraphicsTextItem *highScoreText;

    int score;
    int enemyVelo;
    int level;
    int highScore;

    bool gameStarted;
    bool gamePaused;

    Button *playButton;
    Button *pauseButton;

//    Button *serverButton;

//    ChatServer *server;
//    ChatWindow *chatWindow;

    // local solo game
    // connect to gameStart()
    Button *startLocalGame;
    // initializes server and waits for players to join
    // connect to makeLobby()
    Button *startPublicGame;
    // attempts to join active server
    // connect to joinLobby()
    Button *joinPublicGame;

    // will be used if this computer is host
    ChatServer *server;
    // will be used if this computer is not host
    ChatClient *client;

};

#endif // GAME_H
