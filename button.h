#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsRectItem>

class Button : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Button(QString name);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    void mouseMove(QPointF pos);
    void mousePress(QPointF pos);
    void mouseRelease();

signals:
    void clicked();

private:
    Qt::GlobalColor buttonColor;
    QString buttonName;
    bool pressed;
};

#endif // BUTTON_H
