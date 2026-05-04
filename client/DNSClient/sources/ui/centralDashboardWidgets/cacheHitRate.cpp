#include "../../headers/ui/centralDashboardWidgets/cacheHitRate.h"

CacheHit::CacheHit(QWidget* parent) : QWidget(parent) {
    init();
}

void CacheHit::init() {
    // ------------------------ Begin --------------------------
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    // ------------------------- View --------------------------
    m_cacheHit = new QCustomPlot(this);

    // ------------------------ Title --------------------------
    m_cacheHit->plotLayout()->insertRow(0);
    m_title = new QCPTextElement(
        m_cacheHit,
        "cache hit rate — last 60s",
        QFont("Inner", 8)
        );

    m_title->setTextColor(Qt::gray);
    m_title->setTextFlags(Qt::AlignLeft | Qt::AlignVCenter);

    m_cacheHit->plotLayout()->addElement(0, 0, m_title);

    // ------------------------- Graph -------------------------
    // background
    m_cacheHit->setBackground(Qt::transparent);
    m_cacheHit->axisRect()->setBackground(Qt::transparent);

    // padding
    m_cacheHit->axisRect()->setAutoMargins(QCP::msNone);
    m_cacheHit->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // graph
    m_cacheHit->addGraph();

    m_cacheHit->graph(0)->setPen(QPen(QColor(ACCENT_GREEN), 1));
    m_cacheHit->graph(0)->setBrush(QColor(ACCENT_GREEN_BRUSH));

    m_cacheHit->legend->setVisible(false);

    m_cacheHit->xAxis->setVisible(false);
    m_cacheHit->yAxis->setVisible(false);
    m_cacheHit->xAxis->grid()->setVisible(false);
    m_cacheHit->yAxis->grid()->setVisible(false);
    m_cacheHit->setInteractions(QCP::iNone);

    // range
    m_cacheHit->xAxis->setRange(0, 60);
    m_cacheHit->yAxis->setRange(0, 100);

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
    layout->addWidget(m_cacheHit, 1);
    m_cacheHit->graph(0)->setData(x, y);
    m_cacheHit->replot();
}

