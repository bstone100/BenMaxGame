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

    bool getPressed() const;

    void setButtonName(const QString &newButtonName);

    void setIcon(const QPixmap &newIcon);

    void setFontDivisor(int newFontDivisor);

signals:
    void clicked();

private:
    Qt::GlobalColor buttonColor;
    QString buttonName;
    bool pressed;
    QPixmap icon;
    int fontDivisor;
};

#endif // BUTTON_H
