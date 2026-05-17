#ifndef BASECUSTOMPLOT_H
#define BASECUSTOMPLOT_H

#include <QWidget>

#include "../dependencies/qcustomplot/qcustomplot.h"

class BasePlot : public QWidget {
    static constexpr inline int MAX_POINTS_COUNT = 60;
public:
    explicit BasePlot(const QString& title, const QColor& col, const QColor& gradient, QWidget* parent = nullptr);
    void updateState(double lat);

private:
    void init(const QString& title, const QColor& col, const QColor& gradient);

private:
    uint32_t m_time{};
    double m_lastValue{};
    QVector<double> m_x;
    QVector<double> m_y;

    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_plot = nullptr;
};

#endif // BASECUSTOMPLOT_H
