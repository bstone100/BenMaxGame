#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "button.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

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

    void setScore(int newScore);

private:
    bool upHeld, downHeld, leftHeld, rightHeld;

    QPointF mouseTip, playerCenter, gunTip;

    qreal mouseAngle;

    Player *player;
    HealthBar *playerHealthBar;
    QVector<Bullet *> bullets;
    QVector<Enemy *> enemies;

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

    Button *playButton;
};

#endif // GRAPHICSVIEW_H
