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
protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void logMessage(const QString &msg);
    void playerJoined();
public slots:
    void stopServer();
private slots:
    void broadcast(const QJsonObject &message, ServerWorker *exclude);
    void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
    void userDisconnected(ServerWorker *sender);
    void userError(ServerWorker *sender);
    void dataReceived(ServerWorker *sender, Data &data);
private:
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
    void sendJson(ServerWorker *destination, const QJsonObject &message);
    QVector<ServerWorker *> m_clients;
};

#endif // CHATSERVER_H
