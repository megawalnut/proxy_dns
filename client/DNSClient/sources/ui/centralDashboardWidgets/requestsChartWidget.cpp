#include "../../headers/ui/centralDashboardWidgets/requestsChartWidget.h"

RequestsChart::RequestsChart(QWidget* parent) : QWidget(parent) {
    init();
}

void RequestsChart::init() {
    // ------------------------ Begin --------------------------
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    // ------------------------- View --------------------------
    m_requestPlt = new QCustomPlot(this);

    // ------------------------ Title --------------------------
    m_requestPlt->plotLayout()->insertRow(0);
    m_title = new QCPTextElement(
        m_requestPlt,
        "requests / sec — last 60s",
        QFont("Inner", 8)
    );

    m_title->setTextColor(Qt::gray);
    m_title->setTextFlags(Qt::AlignLeft | Qt::AlignVCenter);

    m_requestPlt->plotLayout()->addElement(0, 0, m_title);

    // ------------------------- Graph -------------------------
    // background
    m_requestPlt->setBackground(Qt::transparent);
    m_requestPlt->axisRect()->setBackground(Qt::transparent);

    // padding
    m_requestPlt->axisRect()->setAutoMargins(QCP::msNone);
    m_requestPlt->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // graph
    m_requestPlt->addGraph();

    m_requestPlt->graph(0)->setPen(QPen(ACCENT_BLUE, 1));
    m_requestPlt->graph(0)->setBrush(ACCENT_BLUE_BRUSH);

    m_requestPlt->legend->setVisible(false);

    m_requestPlt->xAxis->setVisible(false);
    m_requestPlt->yAxis->setVisible(false);
    m_requestPlt->xAxis->grid()->setVisible(false);
    m_requestPlt->yAxis->grid()->setVisible(false);
    m_requestPlt->setInteractions(QCP::iNone);

    // range
    m_requestPlt->xAxis->setRange(0, 60);
    m_requestPlt->yAxis->setRange(0, 100);

    // ------------------------ Model --------------------------
    // delete this
    QVector<double> x(60), y(60);

    for (int i = 0; i < 60; i++) {
        x[i] = i;
        y[i] = rand() % 100;
    }

    // ------------------------ Style --------------------------
    setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: 10px;
            border: 1px solid %2;
        }
    )").arg(BG_CARD.name(), BORDER.name()));

    // ------------------------ Final --------------------------
    layout->addWidget(m_requestPlt, 1);
    m_requestPlt->graph(0)->setData(x, y);
    m_requestPlt->replot();
}
