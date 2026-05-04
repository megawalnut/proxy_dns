#ifndef REQUESTSCHARTWIDGET_H
#define REQUESTSCHARTWIDGET_H

#include <QWidget>

#include "../../../headers/themes/darkTheme.h"
#include "../../../dependencies/qcustomplot/qcustomplot.h"

class RequestsChart : public QWidget {
public:
    explicit RequestsChart(QWidget* parent = nullptr);

private:
    void init();

private:
    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_requestPlt = nullptr;
};

#endif // REQUESTSCHARTWIDGET_H
