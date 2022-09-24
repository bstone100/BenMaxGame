#include "chatwindow.h"
//#include "ui_chatwindow.h"
#include "chatclient.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>
ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , m_chatClient(new ChatClient(this)) // create the chat client
{
    // connect the signals from the chat client to the slots in this ui
    connect(m_chatClient, &ChatClient::connected, this, &ChatWindow::connectedToServer);
    connect(m_chatClient, &ChatClient::loggedIn, this, &ChatWindow::loggedIn);
    connect(m_chatClient, &ChatClient::loginError, this, &ChatWindow::loginFailed);
    connect(m_chatClient, &ChatClient::messageReceived, this, &ChatWindow::messageReceived);
    connect(m_chatClient, &ChatClient::disconnected, this, &ChatWindow::disconnectedFromServer);
    connect(m_chatClient, &ChatClient::error, this, &ChatWindow::error);
    connect(m_chatClient, &ChatClient::userJoined, this, &ChatWindow::userJoined);
    connect(m_chatClient, &ChatClient::userLeft, this, &ChatWindow::userLeft);
    connect(m_chatClient, &ChatClient::posReceived, this, &ChatWindow::posReceived);
}

ChatWindow::~ChatWindow()
{

}

void ChatWindow::attemptConnection()
{
    // We ask the user for the address of the server, we use 127.0.0.1 (aka localhost) as default
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Chose Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral("127.0.0.1")
    );
    if (hostAddress.isEmpty())
        return; // the user pressed cancel or typed nothing
    // tell the client to connect to the host using the port 1967
    m_chatClient->connectToServer(QHostAddress(hostAddress), 1967);
}

void ChatWindow::connectedToServer()
{
    // once we connected to the server we ask the user for what username they would like to use
    const QString newUsername = QInputDialog::getText(this, tr("Chose Username"), tr("Username"));
    if (newUsername.isEmpty()){
        // if the user clicked cancel or typed nothing, we just disconnect from the server
        return m_chatClient->disconnectFromHost();
    }
    // try to login with the given username
    attemptLogin(newUsername);
}

void ChatWindow::attemptLogin(const QString &userName)
{
    // use the client to attempt a log in with the given username
    m_chatClient->login(userName);
}

void ChatWindow::loggedIn()
{
    emit readyToStart();
}

void ChatWindow::loginFailed(const QString &reason)
{
    // the server rejected the login attempt
    // display the reason for the rejection in a message box
    QMessageBox::critical(this, tr("Error"), reason);
    // allow the user to retry, execute the same slot as when just connected
    connectedToServer();
}

void ChatWindow::messageReceived(const QString &sender, const QString &text)
{

}

void ChatWindow::sendMessage(const QPointF &pos)
{
    // we use the client to send the message that the user typed
    m_chatClient->sendMessage(pos);
}

void ChatWindow::disconnectedFromServer()
{
    // if the client loses connection to the server
    // comunicate the event to the user via a message box
    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));
}

void ChatWindow::userJoined(const QString &username)
{
    emit playerJoined(username);
}
void ChatWindow::userLeft(const QString &username)
{
    emit playerLeft(username);
}

void ChatWindow::error(QAbstractSocket::SocketError socketError)
{
    // show a message to the user that informs of what kind of error occurred
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return; // handled by disconnectedFromServer
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }
}

void ChatWindow::endGame()
{
    m_chatClient->disconnectFromHost();
}

void ChatWindow::posReceived(QPointF pos)
{
    emit playerMoved(pos);
}
