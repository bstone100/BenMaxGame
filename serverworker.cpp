#include "serverworker.h"
#include "QtCore/qpoint.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include "data.h"

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveData);
    // forward the disconnected and error signals coming from the socket
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
#else
    connect(m_serverSocket, &QAbstractSocket::errorOccurred, this, &ServerWorker::error);
#endif
}


bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::sendData(Data &data)
{
    QDataStream socketStream(m_serverSocket);
    socketStream << data;
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->disconnectFromHost();
}

void ServerWorker::receiveData()
{
    QDataStream socketStream(m_serverSocket);
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

QTcpSocket *ServerWorker::serverSocket() const
{
    return m_serverSocket;
}


