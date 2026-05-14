#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "../../headers/common/utils.h"

class ToolBar : public QWidget {
    Q_OBJECT
public:
    explicit ToolBar(QWidget* parent = nullptr);
    void updateState(Utils::Areas::ToolBarData);
    void disableUI();

private:
    void init();

    void setStatus(bool state);
    void setUptime(double msec);
    void setRPC(uint32_t rpc);
    void setCacheHit(double hits);
    void setLatency(double p95);
    void setResolveAvg(double resAvg);
    void setErrorsPercent(double errs);

private:
    QLabel* m_status = nullptr;
    QLabel* m_uptime = nullptr;
    QLabel* m_rps = nullptr;
    QLabel* m_cacheHits = nullptr;
    QLabel* m_latency = nullptr;
    QLabel* m_resAvg = nullptr;
    QLabel* m_errors = nullptr;
};
#endif // TOOLBAR_H
