#ifndef CACHEHITRATE_H
#define CACHEHITRATE_H

#include <QWidget>

#include "../../../dependencies/qcustomplot/qcustomplot.h"

class CacheHit : public QWidget {
    static constexpr inline int MAX_POINTS_COUNT = 60;
public:
    explicit CacheHit(QWidget* parent = nullptr);
    void updateState(double cacheHit);

private:
    void init();

private:
    uint32_t m_time{};
    QVector<double> m_x;
    QVector<double> m_y;

    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_cacheHit = nullptr;
};

#endif // CACHEHITRATE_H
