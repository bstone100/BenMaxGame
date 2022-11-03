#include "chatclient.h"
#include "QtCore/qpoint.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPointF>
#include "data.h"

ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
    // Forward the connected and disconnected signals
    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &ChatClient::disconnected);
    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::dataIncoming);
    // Forward the error signal, QOverload is necessary as error() is overloaded, see the Qt docs
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatClient::error);
#else
    connect(m_clientSocket, &QAbstractSocket::errorOccurred, this, &ChatClient::error);
#endif
    // Reset the m_loggedIn variable when we disconnec. Since the operation is trivial we use a lambda instead of creating another slot
    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});
}

void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);
}

void ChatClient::dataIncoming()
{
    QDataStream socketStream(m_clientSocket);
    Data gameData;
    for (;;) {
        socketStream.startTransaction();
        socketStream >> gameData;
        if (socketStream.commitTransaction()) {
            emit dataReceived(gameData);
        } else {
            break;
        }
    }
}

QTcpSocket *ChatClient::clientSocket() const
{
    return m_clientSocket;
}
