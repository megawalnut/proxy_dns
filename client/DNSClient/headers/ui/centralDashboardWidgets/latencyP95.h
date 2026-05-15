#ifndef LATENCYP95_H
#define LATENCYP95_H

#include <QWidget>

#include "../../../dependencies/qcustomplot/qcustomplot.h"

class Latency : public QWidget {
    static constexpr inline int MAX_POINTS_COUNT = 60;
public:
    explicit Latency(QWidget* parent = nullptr);
    void updateState(double lat);
    void disableUI();

private:
    void init();

private:
    uint32_t m_time{};
    QVector<double> m_x;
    QVector<double> m_y;

    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_latency = nullptr;
};

#endif // LATENCYP95_H
