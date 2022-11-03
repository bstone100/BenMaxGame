#include "chatserver.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>
ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent)
{
    serverSize = 2;
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&ChatServer::userDisconnected, this, worker));
    connect(worker, &ServerWorker::dataReceived, this, std::bind(&ChatServer::dataReceived, this, worker, std::placeholders::_1));
    m_clients.append(worker);

    if (m_clients.size() == serverSize) emit serverFull(serverSize);
}

void ChatServer::dataReceived(ServerWorker *sender, Data &data)
{
    Q_ASSERT(sender);
    for (ServerWorker *worker : m_clients) {
        Q_ASSERT(worker);
        if (worker == sender)
            continue;
        worker->sendData(data);
    }
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeOne(sender);
    sender->deleteLater();
}

int ChatServer::getServerSize() const
{
    return serverSize;
}

void ChatServer::setServerSize(int newServerSize)
{
    serverSize = newServerSize;
}

QVector<ServerWorker *> &ChatServer::clients()
{
    return m_clients;
}

void ChatServer::stopServer()
{
    for (ServerWorker *worker : m_clients) {
        worker->disconnectFromClient();
    }
    close();
}


