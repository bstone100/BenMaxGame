#include "button.h"
#include "QtGui/qpainter.h"
#include "QGraphicsSceneMouseEvent"
#include "QtWidgets/qapplication.h"
#include <QCursor>

QColor textColor(45, 33 ,90);

Button::Button(QString primaryName, QString altName, bool wait)
    : buttonName(primaryName), primaryName(primaryName), altName(altName),
      waitForSomething(wait)
{
    setRect(0, 0, 200, 100);
    buttonColor = Qt::white;
    pressed = false;
    fontDivisor = 4;
    enabled = true;
    setBoundingRegionGranularity(1);
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!enabled) {
        painter->setOpacity(.5);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(buttonColor);
    double num = 0;
    rect().height() < rect().width() ? num = rect().height() : num = rect().width();
    num *= .25;
    painter->drawRoundedRect(rect(), num, num);

    painter->setPen(textColor);
    painter->setFont(QFont("Arial", rect().width() / fontDivisor, QFont::Bold));
    painter->drawText(rect(), Qt::AlignCenter, buttonName);

    if (!icon.isNull())
        painter->drawPixmap(rect(), icon, icon.rect());

}

bool Button::mouseMove(QPointF pos)
{
    bool contains = boundingRegion(sceneTransform()).contains(pos.toPoint());

    if (contains) {
        if (pressed) {
            buttonColor = Qt::gray;
        } else {
            buttonColor = Qt::lightGray;
        }
    } else {
        buttonColor = Qt::white;
    }
    update();

    return contains;
}

void Button::mousePress(QPointF pos)
{
    if (boundingRegion(sceneTransform()).contains(pos.toPoint())) {
        buttonColor = Qt::gray;
        pressed = true;
        update();
    }
}

void Button::mouseRelease()
{
    if (pressed) {
        if (buttonColor == Qt::gray && enabled) {
            if (buttonName == primaryName) {
                if (!waitForSomething)
                    buttonName = altName;
            } else if (buttonName == altName) {
                buttonName = primaryName;
            }
            emit clicked();
        }
        buttonColor = Qt::white;
        pressed = false;
        update();
    }
}

bool Button::getPressed() const
{
    return pressed;
}

void Button::setButtonName(const QString &newButtonName)
{
    buttonName = newButtonName;
    update();
}

void Button::setIcon(const QPixmap &newIcon)
{
    icon = newIcon;
}

void Button::setFontDivisor(int newFontDivisor)
{
    fontDivisor = newFontDivisor;
}

bool Button::getEnabled() const
{
    return enabled;
}

void Button::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
    update();
}

void Button::setNameAlt()
{
    buttonName = altName;
    update();
}

void Button::setNamePrimary()
{
    buttonName = primaryName;
    update();
}

void Button::reset()
{
    buttonName = primaryName;
    enabled = true;
    pressed = false;
    buttonColor = Qt::white;
    update();
}
