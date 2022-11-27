#include "timer.h"
#include "QtCore/qdebug.h"

Timer::Timer(QObject *parent)
    : QTimer{parent}
{

}

Timer::Timer(QObject *parent, int interval, bool singleShot)
    : QTimer(parent), oldInterval(interval), singleShot(singleShot)
{
    setInterval(interval);
    setSingleShot(singleShot);
}

// use instead of setInterval()
void Timer::changeInterval(int newInterval)
{
    oldInterval = newInterval;
    setInterval(newInterval);
}

// use instead of setSingleShot()
void Timer::changeSingleShot(bool newSingleShot)
{
    singleShot = newSingleShot;
    setSingleShot(newSingleShot);
}

// save remaining time
void Timer::pause()
{
//    qDebug() << "pause";

    remaining = remainingTime();
    stop();
}

// run for remaining time
void Timer::resume()
{
    if (remaining < 0) return;

//    qDebug() << "resume: " << remaining << " ms remaining";

    setInterval(remaining);
    setSingleShot(true);
    QObject::connect(this, &QTimer::timeout, this, &Timer::reset);
    start();
}

// return timer to state before pause
void Timer::reset()
{
//    qDebug() << "resetting to " << oldInterval << " ms";

    setInterval(oldInterval);
    setSingleShot(singleShot);
    QObject::disconnect(this, &QTimer::timeout, this, &Timer::reset);
    if (!singleShot) {
        start();
    }
}
