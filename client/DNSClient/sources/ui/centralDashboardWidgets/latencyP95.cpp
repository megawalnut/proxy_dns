#include "../../headers/ui/centralDashboardWidgets/latencyP95.h"
#include "../../../headers/themes/darkTheme.h"

Latency::Latency(QWidget* parent) : QWidget(parent) {
    init();
}

void Latency::init() {
    // ------------------------ Begin --------------------------
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    // ------------------------- View ---------------------------
    m_latency = new QCustomPlot(this);

    // ------------------------ Title --------------------------
    m_latency->plotLayout()->insertRow(0);
    m_title = new QCPTextElement(
        m_latency,
        "latency p95 ms — last 60s",
        QFont("Inner", 8)
        );

    m_title->setTextColor(Qt::gray);
    m_title->setTextFlags(Qt::AlignLeft | Qt::AlignVCenter);

    m_latency->plotLayout()->addElement(0, 0, m_title);

    // ------------------------- Graph -------------------------
    // background
    m_latency->setBackground(Qt::transparent);
    m_latency->axisRect()->setBackground(Qt::transparent);

    // padding
    m_latency->axisRect()->setAutoMargins(QCP::msNone);
    m_latency->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // graph
    m_latency->addGraph();

    m_latency->graph(0)->setPen(QPen(QColor(ACCENT_AMBER), 1));
    m_latency->graph(0)->setBrush(QColor(ACCENT_AMBER_BRUSH));

    m_latency->legend->setVisible(false);

    m_latency->xAxis->setVisible(false);
    m_latency->yAxis->setVisible(false);
    m_latency->xAxis->grid()->setVisible(false);
    m_latency->yAxis->grid()->setVisible(false);
    m_latency->setInteractions(QCP::iNone);

    // ------------------------ Style --------------------------
    setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: 10px;
            border: 1px solid %2;
        }
    )").arg(BG_CARD.name(), BORDER.name()));

    // ------------------------ Final --------------------------
    layout->addWidget(m_latency, 1);
}

void Latency::updateState(double lat) {
    if(lat > 0) {
        m_x.push_back(++m_time);
        m_y.push_back(lat);

        if(m_x.size() >= MAX_POINTS_COUNT) {
            m_x.removeFirst();
            m_y.removeFirst();
        }

        m_latency->graph(0)->setData(m_x, m_y);
        m_latency->xAxis->setRange(m_x.first(), m_x.last());

        double max = *std::max_element(m_y.begin(), m_y.end());
        m_latency->yAxis->setRange(0, max > 0 ? max * 1.2 : 10);

        m_latency->replot();
    }
}

void Latency::disableUI() {
    m_x.push_back(++m_time);
    m_y.push_back(0);

    if(m_x.size() >= MAX_POINTS_COUNT) {
        m_x.removeFirst();
        m_y.removeFirst();
    }

    m_latency->graph(0)->setData(m_x, m_y);

    m_latency->xAxis->setRange(m_x.first(), m_x.last());
    m_latency->yAxis->setRange(0, 0);

    m_latency->replot();
}
