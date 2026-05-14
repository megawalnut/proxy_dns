#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>


class Client final : public QObject {
    static constexpr inline const char* SERVER_ADDR = "127.0.0.1";
    static constexpr inline quint16 SERVER_PORT = 6060;
    Q_OBJECT
public:
    explicit Client(const quint16 portNum = SERVER_PORT,
                    const QString& serverAddress = SERVER_ADDR,
                    QObject* parent = nullptr);

    void startConnection();
    bool connected() {
        return m_serverSocket && m_serverSocket->state() == QAbstractSocket::ConnectedState;
    }

private:
    void setupConnections();

private slots:
    void onReadyRead();

    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError);
    void onDisconnected();

    void scheduleReconnect();

signals:
    void packetReady(QJsonObject);
    void startUI();
    void stopUI();

private:
    QTcpSocket* m_serverSocket = nullptr;
    bool m_reconnecting;
    QByteArray m_buffer;
    QString m_serverAddr; // "127.0.0.1"
    quint16 m_portNum;    // 6060
};

#endif // CLIENT_H
