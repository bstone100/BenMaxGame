#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include "data.h"
#include <QObject>
#include <QTcpSocket>
#include <QPointF>
class QHostAddress;
class QJsonDocument;
class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
    QTcpSocket *clientSocket() const;
public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void disconnectFromHost();
private slots:
    void dataIncoming();
signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &sender, const QString &text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void posReceived(QPointF pos);
    void dataReceived(Data data);
private:
    QTcpSocket *m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QJsonObject &doc);
};

#endif // CHATCLIENT_H
