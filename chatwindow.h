#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QAbstractSocket>
class ChatClient;
class QStandardItemModel;
namespace Ui { class ChatWindow; }
class ChatWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatWindow)
public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();
private:
    ChatClient *m_chatClient;
signals:
    void readyToStart();
    void playerJoined(const QString &text);
    void playerLeft(const QString &text);
    void playerMoved(QPointF pos);

public slots:
    void attemptConnection();
    void connectedToServer();
    void attemptLogin(const QString &userName);
    void loggedIn();
    void loginFailed(const QString &reason);
    void messageReceived(const QString &sender, const QString &text);
    void sendMessage(const QPointF &pos);
    void disconnectedFromServer();
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void error(QAbstractSocket::SocketError socketError);

    void endGame();
    void posReceived(QPointF pos);
};

#endif // CHATWINDOW_H
