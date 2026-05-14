#include "../../headers/controller/dashboardController.h"

DashboardController::DashboardController(Client* client, QObject* parent)
    :
    QObject(parent),
    m_client(client)
{
    setupConnections();
}

void DashboardController::setupConnections() {
    connect(m_client, &Client::startUI,
            this, &DashboardController::startUI);

    connect(m_client, &Client::stopUI,
            this, &DashboardController::stopUI);

    connect(m_client, &Client::packetReady,
            this, &DashboardController::onPacketReady);
}

void DashboardController::onPacketReady(QJsonObject obj) {
#ifdef QT_DEBUG
    qDebug() << "\n-----------------------------------------------------------------------\n";
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        qDebug() << it.key() << ":" << it.value();
    }
#endif

    auto sn = Utils::parseSnapshot(obj);

    emit toolBarUpdate(Utils::parseToolBarData(sn));
    emit centralDataUpdate(Utils::parseCentralData(sn));
    emit statusBarUpdate(Utils::parseStatusBarData(sn));
}
