#ifndef REQUESTSCHARTWIDGET_H
#define REQUESTSCHARTWIDGET_H

#include <QWidget>

#include "../../../dependencies/qcustomplot/qcustomplot.h"

class RequestsChart : public QWidget {
    static constexpr inline int MAX_POINTS_COUNT = 60;
public:
    explicit RequestsChart(QWidget* parent = nullptr);
    void updateState(double requests);

private:
    void init();

private:
    uint32_t m_time{};
    QVector<double> m_x;
    QVector<double> m_y;

    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_requests = nullptr;
};

#endif // REQUESTSCHARTWIDGET_H
