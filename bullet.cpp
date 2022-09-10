#include "bullet.h"
#include "QtWidgets/qgraphicsscene.h"
#include "graphicsview.h"

Bullet::Bullet(QPointF gunTip, qreal angle)
{
    size = 8;
    velo = 10;
    this->angle = angle;
    setRect(0, 0, size, size);
    setPos(gunTip.x() - size / 2, gunTip.y() - size / 2);
}

void Bullet::advance(int step)
{
    if (!step) return;

    moveBy(velo * cos(angle), -velo * sin(angle));

    QRectF circleBorder(x(), y(), size, size);
    QRectF sceneBorder(-size, -size, scene()->width() + size * 2, scene()->height() + size * 2);
    bool contains = sceneBorder.contains(circleBorder);

    if (!contains) {
        scene()->removeItem(this);
        delete this;
//        QGraphicsView *view = scene()->views().at(0);
//        GraphicsView *v = static_cast<GraphicsView *>(view);
//        if (v) {
//            v->removeBullet(this);
//            delete this;
//        }
    }
}
