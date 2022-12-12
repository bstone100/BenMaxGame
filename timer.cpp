#include "timer.h"
#include <QTimerEvent>
#include <QAbstractEventDispatcher>

static const int INV_TIMER = -1;

Timer::Timer(QObject *parent, int interval, bool singleShot)
    : QObject{parent}, inter(interval), single(singleShot)
{
    id = INV_TIMER;
    resuming = false;
    remaining = -1;
}

Timer::~Timer()
{
    if (id != INV_TIMER)
        stop();
}

void Timer::start()
{
    if (id != INV_TIMER)
        stop();
    id = QObject::startTimer(inter, Qt::PreciseTimer);
}

void Timer::start(int msec)
{
    inter = msec;
    start();
}

void Timer::stop()
{
    if (id != INV_TIMER) {
        QObject::killTimer(id);
        id = INV_TIMER;
    }
}

void Timer::pause()
{
    remaining = remainingTime();
    stop();
}

void Timer::resume()
{
    if (isActive() || remaining < 0) return;

    resuming = true;
    id = QObject::startTimer(remaining, Qt::PreciseTimer);
    remaining = -1;
}

void Timer::setInterval(int msec)
{
    inter = msec;
    if (id != INV_TIMER) {
        QObject::killTimer(id);
        id = QObject::startTimer(msec, Qt::PreciseTimer);
    }
}

int Timer::remainingTime() const
{
    if (id != INV_TIMER) {
        return QAbstractEventDispatcher::instance()->remainingTime(id);
    }
    return -1;
}

void Timer::setSingleShot(bool singleShot)
{
    single = singleShot;
}

void Timer::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == id) {
        if (resuming) {
            stop();
            if (!single)
                start();
            resuming = false;
        } else {
            if (single)
                stop();
        }
        emit timeout();
    }
}
