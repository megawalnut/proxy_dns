#ifndef DASHBOARDCONTROLLER_H
#define DASHBOARDCONTROLLER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../headers/net/client.h"
#include "../../headers/common/utils.h"

class DashboardController final : public QObject {
    Q_OBJECT
public:
    explicit DashboardController(Client* client,
                                 QObject* parent = nullptr);

private:
    void setupConnections();

private slots:
    void onPacketReady(QJsonObject);

signals:
    void startUI();
    void serverUnavailable();

    void toolBarUpdate(Utils::Areas::ToolBarData);
    void centralDataUpdate(Utils::Areas::CentralData);
    void statusBarUpdate(Utils::Areas::StatusBarData);

private:
    Client* m_client = nullptr;
};

#endif // DASHBOARDCONTROLLER_H
