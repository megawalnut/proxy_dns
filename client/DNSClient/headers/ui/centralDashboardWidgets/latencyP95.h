#ifndef LATENCYP95_H
#define LATENCYP95_H

#include <QWidget>
#include "../../../dependencies/qcustomplot/qcustomplot.h"
#include "../../../headers/themes/darkTheme.h"

class Latency : public QWidget {
public:
    explicit Latency(QWidget* parent = nullptr);

private:
    void init();

private:
    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_latency = nullptr;
};

#endif // LATENCYP95_H
