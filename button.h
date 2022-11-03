#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsRectItem>

class Button : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Button(QString primaryName, QString altName, bool wait = false);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    void mouseMove(QPointF pos);
    void mousePress(QPointF pos);
    void mouseRelease();

    bool getPressed() const;

    void setButtonName(const QString &newButtonName);

    void setIcon(const QPixmap &newIcon);

    void setFontDivisor(int newFontDivisor);

    bool getEnabled() const;
    void setEnabled(bool newEnabled);

    void setNameAlt();
    void setNamePrimary();
    void reset();

signals:
    void clicked();

private:
    Qt::GlobalColor buttonColor;

    QString buttonName;
    QString primaryName;
    QString altName;

    bool waitForSomething;

    bool pressed;
    bool enabled;
    QPixmap icon;
    int fontDivisor;
};

#endif // BUTTON_H
