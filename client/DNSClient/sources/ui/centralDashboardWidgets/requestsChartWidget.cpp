#include "../../headers/ui/centralDashboardWidgets/requestsChartWidget.h"
#include "../../../headers/themes/darkTheme.h"

RequestsChart::RequestsChart(QWidget* parent) : QWidget(parent) {
    init();
}

void RequestsChart::init() {
    // ------------------------ Begin --------------------------
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    // ------------------------- View --------------------------
    m_requests = new QCustomPlot(this);

    // ------------------------ Title --------------------------
    m_requests->plotLayout()->insertRow(0);
    m_title = new QCPTextElement(
        m_requests,
        "requests / sec — last 60s",
        QFont("Inner", 8)
    );

    m_title->setTextColor(Qt::gray);
    m_title->setTextFlags(Qt::AlignLeft | Qt::AlignVCenter);

    m_requests->plotLayout()->addElement(0, 0, m_title);

    // ------------------------- Graph -------------------------
    // background
    m_requests->setBackground(Qt::transparent);
    m_requests->axisRect()->setBackground(Qt::transparent);

    // padding
    m_requests->axisRect()->setAutoMargins(QCP::msNone);
    m_requests->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // graph
    m_requests->addGraph();

    m_requests->graph(0)->setPen(QPen(ACCENT_BLUE, 1));
    m_requests->graph(0)->setBrush(ACCENT_BLUE_BRUSH);

    m_requests->legend->setVisible(false);

    m_requests->xAxis->setVisible(false);
    m_requests->yAxis->setVisible(false);
    m_requests->xAxis->grid()->setVisible(false);
    m_requests->yAxis->grid()->setVisible(false);
    m_requests->setInteractions(QCP::iNone);

    // ------------------------ Style --------------------------
    setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: 10px;
            border: 1px solid %2;
        }
    )").arg(BG_CARD.name(), BORDER.name()));

    // ------------------------ Final --------------------------
    layout->addWidget(m_requests, 1);
}

void RequestsChart::updateState(double requests) {
    if(requests > 0) {
        m_x.push_back(++m_time);
        m_y.push_back(requests);

        if(m_x.size() >= MAX_POINTS_COUNT) {
            m_x.removeFirst();
            m_y.removeFirst();
        }

        m_requests->graph(0)->setData(m_x, m_y);
        m_requests->xAxis->setRange(m_x.first(), m_x.last());

        double max = *std::max_element(m_y.begin(), m_y.end());
        m_requests->yAxis->setRange(0, max > 0 ? max * 1.2 : 10);

        m_requests->replot();
    }
}

void RequestsChart::disableUI() {
    m_x.push_back(++m_time);
    m_y.push_back(0);

    if(m_x.size() >= MAX_POINTS_COUNT) {
        m_x.removeFirst();
        m_y.removeFirst();
    }

    m_requests->graph(0)->setData(m_x, m_y);

    m_requests->xAxis->setRange(m_x.first(), m_x.last());
    m_requests->yAxis->setRange(0, 0);

    m_requests->replot();
}
