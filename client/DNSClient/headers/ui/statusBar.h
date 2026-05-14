#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QVBoxLayout>
#include <QLabel>
#include <QSvgWidget>
#include <QStatusBar>

#include "../../headers/common/utils.h"

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = nullptr);
    void updateState(Utils::Areas::StatusBarData);
    void disableUI();

private:
    void init();

    void setCacheEntries(uint64_t entries);
    void setTotalRequests(uint64_t reqsts);
    void setRam(uint64_t rap);
    void setCpu(double cpu);
    void setThreads(uint32_t thrds);

private:
    QLabel* m_cacheEntries = nullptr;
    QLabel* m_requests = nullptr;
    QLabel* m_ram = nullptr;
    QLabel* m_cpu = nullptr;
    QLabel* m_threads = nullptr;
    QSvgWidget* m_logo = nullptr;
};
#endif // STATUSBAR_H
