#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "gun.h"
#include "player.h"
#include "bullet.h"
#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView();
    GraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


    Player *getPlayer() const;
    void setPlayer(Player *newPlayer);

    Gun *getGun() const;
    void setGun(Gun *newGun);

private:
    bool upPersistent, downPersistent, leftPersistent, rightPersistent;
    Player *player;
    Gun *gun;
    QVector<Bullet *> bullets;
};

#endif // GRAPHICSVIEW_H
