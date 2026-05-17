#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include  "../headers/common/utils.h"

#include "../headers/basicTypes/baseCustomPlot.h"
//#include "../headers/ui/centralDashboardWidgets/requestsChartWidget.h"
//#include "../headers/ui/centralDashboardWidgets/cacheHitRate.h"
//#include "../headers/ui/centralDashboardWidgets/latencyP95.h"
#include "../headers/ui/centralDashboardWidgets/topDomains.h"
#include "../headers/ui/centralDashboardWidgets/querryTypes.h"
#include "../headers/ui/centralDashboardWidgets/recentErrors.h"

class CentralWidget : public QWidget {
    Q_OBJECT
public:
    explicit CentralWidget(QWidget* parent = nullptr);
    void updateState(Utils::Areas::CentralData);

private:
    void init();

private:
    QGridLayout* m_grid = nullptr;
    BasePlot* m_request = nullptr;
    BasePlot* m_cacheHit = nullptr;
    BasePlot* m_p95 = nullptr;
    TopDomains* m_domains = nullptr;
    QuerryTypes* m_types = nullptr;
    Errors* m_errors = nullptr;
};
#endif // CENTRALWIDGET_H

