#include "button.h"
#include "QtGui/qpainter.h"
#include "QGraphicsSceneMouseEvent"

QColor textColor(45, 33 ,90);

Button::Button(QString name)
{
    setRect(0, 0, 200, 100);
    buttonName = name;
    buttonColor = Qt::white;
    pressed = false;
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(buttonColor);
    painter->drawRoundedRect(rect(), 20, 20);

    painter->setPen(textColor);
    painter->setFont(QFont("Arial", 50, QFont::Bold));
    painter->drawText(rect(), Qt::AlignCenter, buttonName);
}

void Button::mouseMove(QPointF pos)
{
    if (pressed) {
        if (sceneBoundingRect().contains(pos)) {
            buttonColor = Qt::gray;
        } else {
            buttonColor = Qt::white;
        }
        update();
    }
}

void Button::mousePress(QPointF pos)
{
    if (sceneBoundingRect().contains(pos)) {
        buttonColor = Qt::gray;
        pressed = true;
        update();
    }
}

void Button::mouseRelease()
{
    if (pressed) {
        if (buttonColor == Qt::gray)
            emit clicked();
        buttonColor = Qt::white;
        pressed = false;
    }
}
