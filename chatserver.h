#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "data.h"
#include <QVector>
#include <QTcpServer>
class QThread;
class ServerWorker;
class ChatServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)
public:
    explicit ChatServer(QObject *parent = nullptr);
    int clientNumber(){return m_clients.size();}
    QVector<ServerWorker *> &clients();

    int getServerSize() const;
    void setServerSize(int newServerSize);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void serverFull(int totalPlayers);
    void serverEmpty();
public slots:
    void stopServer();
private slots:
    void dataReceived(ServerWorker *sender, Data &data);
    void userDisconnected(ServerWorker *sender);
private:
    QVector<ServerWorker *> m_clients;
    int serverSize;
};

#endif // CHATSERVER_H
