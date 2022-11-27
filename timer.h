#ifndef TIMER_H
#define TIMER_H

#include <QTimer>

class Timer : public QTimer
{
public:
    explicit Timer(QObject *parent = nullptr);
    Timer(QObject *parent = nullptr, int interval = 0, bool singleShot = false);

    void pause();
    void resume();

    void changeInterval(int newInterval);
    void changeSingleShot(bool newSingleShot);

private:
    int remaining;
    int oldInterval;
    bool singleShot;

    void reset();
};

#endif // TIMER_H
