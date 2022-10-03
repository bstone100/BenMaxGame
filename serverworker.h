#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "data.h"
#include <QObject>
#include <QTcpSocket>
class QJsonObject;
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    void sendData(Data &data);
    QTcpSocket *serverSocket() const;

signals:
    void disconnectedFromClient();
    void error();
    void dataReceived(Data &data);
public slots:
    void disconnectFromClient();
private slots:
    void receiveData();
private:
    QTcpSocket *m_serverSocket;
};

#endif // SERVERWORKER_H
