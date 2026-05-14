#include "../headers/net/client.h"

Client::Client(const quint16 portNum,
               const QString& serverAddress,
               QObject* parent)
    :
    QObject(parent),
    m_portNum(portNum),
    m_serverAddr(serverAddress),
    m_reconnecting(false)
{
    m_serverSocket = new QTcpSocket(this);
    setupConnections();

    qDebug() << "Client::client: Client created";
}

void Client::startConnection() {
    qDebug() << "Client::client: Connection...";
    if(m_serverSocket->state() == QAbstractSocket::UnconnectedState) {
        m_serverSocket->connectToHost(m_serverAddr, m_portNum);
    }
}

void Client::setupConnections() {
    connect(m_serverSocket, &QTcpSocket::readyRead,
            this, &Client::onReadyRead);

    connect(m_serverSocket, &QTcpSocket::connected,
            this, &Client::onConnected);

    connect(m_serverSocket, &QTcpSocket::disconnected,
            this, &Client::onDisconnected);

    connect(m_serverSocket, &QTcpSocket::errorOccurred,
            this, &Client::onErrorOccurred);
}

void Client::onReadyRead() {
    //read from socket and save
    m_buffer.append(m_serverSocket->readAll());

    //if received a part of the package < 4b
    while(m_buffer.contains('\n')) {

        int lastIndex = m_buffer.indexOf('\n');
        QByteArray acceptedPackage = m_buffer.left(lastIndex);   //package data
        m_buffer.remove(0, lastIndex + 1);

        QJsonParseError ok;
        QJsonDocument json =  QJsonDocument::fromJson(acceptedPackage, &ok);
        if(ok.error != QJsonParseError::ParseError::NoError) {
            qWarning() << "Client::onReadyRead: Failed parse to JSON";
            continue;
        }

        qDebug() << "------------------------------------------------------------------------";
        QJsonObject obj = json.object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            qDebug() << it.key() << ":" << it.value();
        }

        emit receivePacket(json.object());
    }
}

void Client::onConnected() {
    qDebug() << QString("Client::onConnected: Connected to server on address %1:%2")
                    .arg(m_serverAddr)
                    .arg(m_portNum);

    m_reconnecting = false;
}

void Client::onErrorOccurred(QAbstractSocket::SocketError err) {
    switch(err) {
    case QAbstractSocket::HostNotFoundError:
        qWarning() << "Client::onError: Error: The host was not found";
        scheduleReconnect();
        return;
    case QAbstractSocket::RemoteHostClosedError:
        qWarning() << "Client::onError: Error: The remote host is closed";
        return;
    case QAbstractSocket::ConnectionRefusedError:
        qWarning() << "Client::onError: Error: The connection was refused";
        scheduleReconnect();
        return;
    default:
        qWarning() << "Client::onError: Error:" << m_serverSocket->errorString();
        return;
    }
}

void Client::onDisconnected() {
    qDebug() << QString("Client::onDisconnected: Disconnected from server %1:%2")
                    .arg(m_serverAddr)
                    .arg(m_portNum);

    m_buffer.clear();
    if (m_reconnecting) {
        return;
    }

    scheduleReconnect();
}

void Client::scheduleReconnect() {
    qWarning() << "Client::scheduleReconnect";
    m_reconnecting = true;

    QTimer::singleShot(5000, this, [this]() {
        if(m_serverSocket->state() == QAbstractSocket::UnconnectedState) {
            m_serverSocket->connectToHost(m_serverAddr, m_portNum);
        }
    });
}
