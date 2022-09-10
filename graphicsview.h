#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "player.h"
#include "bullet.h"
#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    Player *getPlayer() const;
    void setPlayer(Player *newPlayer);

    void shoot();
    void startFullAuto();
    void removeBullet(Bullet *bullet){bullets.removeOne(bullet);}

public slots:
    void moveGun();

private:
    bool upHeld, downHeld, leftHeld, rightHeld;

    QPointF mouseTip, playerCenter, gunTip;

    qreal mouseAngle;

    Player *player;
    QVector<Bullet *> bullets;

    QTimer *shotsTimer;
    QTimer *delayTimer;

    QGraphicsLineItem *xAxis;
    QGraphicsLineItem *yAxis;
    QGraphicsRectItem *box;
};

#endif // GRAPHICSVIEW_H
