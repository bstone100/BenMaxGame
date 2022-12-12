#ifndef TIMER_H
#define TIMER_H

#include <QObject>

class Timer : public QObject
{
    Q_OBJECT
public:
    Timer(QObject *parent = nullptr, int interval = 0, bool singleShot = false);
    ~Timer();

    void start();
    void start(int msec);
    void stop();

    void pause();
    void resume();

    inline bool isActive() const { return id >= 0; }
    int timerId() const { return id; }

    void setInterval(int msec);
    int interval() const { return inter; }

    int remainingTime() const;

    inline void setSingleShot(bool singleShot);
    inline bool isSingleShot() const { return single; }

signals:
    void timeout();

protected:
    void timerEvent(QTimerEvent *e) override;

private:
    int id;
    int inter;
    int remaining;
    bool single;
    bool resuming;

};

#endif // TIMER_H
