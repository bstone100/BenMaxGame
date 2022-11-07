#ifndef GAME_H
#define GAME_H

#include "button.h"
#include "chatclient.h"
#include "chatserver.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include <QGraphicsView>
#include <QElapsedTimer>

class Game : public QGraphicsView
{
    Q_OBJECT
public:
    enum GameMode {
        Solo,
        Multiplayer
    };

    enum Status {
        Host,
        Guest
    };

    Game(QGraphicsScene *scene, QWidget *parent = nullptr);

    void mainFunction();

    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *) override;
    void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio);

    void shoot();
    void startFullAuto();
    void startHealthRegen();
    void healthRegen();
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
    void setFps();
    void resetHS();

    void startSoloGame();
    void startServerGame();

    void toggleStartServer();
    void attemptConnection();
    void error(QAbstractSocket::SocketError socketError);

    void sendPlayerData();
    void sendNewPlayerData();
    void sendNewBulletData(QPointF gunTip, qreal angle);
    void sendNewEnemyData(QPointF startPoint, QPointF playerCenter, int velo, int size);
    void sendServerFull(int totalPlayers);
    void sendGameOver();

    void receiveData(Data data);

    void connectedToServer();
    void disconnectedFromServer();

    void changeName();

private:
    GameMode mode;
    Status status;

    int serverSize;

    bool upHeld, downHeld, leftHeld, rightHeld;

    QPointF mouseTip, playerCenter, gunTip;

    Player *player;
    HealthBar *playerHealthBar;


    QVector<Bullet *> bullets;
    Bullet *newestBullet;

    QVector<Enemy *> enemies;
    Enemy *newestEnemy;

    QMap<QUuid, Player *> otherPlayersMap;

    QTimer *mainTimer;
    QTimer *shotsTimer;
    QTimer *delayTimer;
    QTimer *cleanUpTimer;
    QTimer *makeEnemyTimer;
    QTimer *bulletImpactTimer;
    QTimer *regenDelayTimer;
    QTimer *regenTimer;

    QTimer *fpsTimer;
    QElapsedTimer *fpsStopwatch;
    QVector<int> frameTimes;

    QThread *bulletThread;
    QThread *enemyThread;

    QGraphicsLineItem *xAxis;
    QGraphicsLineItem *yAxis;
    QGraphicsRectItem *box;

    QPixmap background;

    QGraphicsTextItem *title;
    QGraphicsTextItem *scoreText;
    QGraphicsTextItem *highScoreText;
    QGraphicsTextItem *fpsText;

    int enemyVelo;
    int level;
    int highScore;

    bool gameStarted;
    bool gamePaused;

    Button *pauseButton;

    Button *startLocalGame;
    Button *startPublicGame;
    Button *joinPublicGame;
    Button *changeNameButton;
    QVector<Button *> buttons;

    // will be used if this computer is host
    ChatServer *server;

    ChatClient *client;

};

#endif // GAME_H
