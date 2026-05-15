#include "../../headers/ui/centralDashboardWidgets/cacheHitRate.h"
#include "../../../headers/themes/darkTheme.h"

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
}

void CacheHit::updateState(double cacheHit) {
    if(cacheHit > 0) {
        m_x.push_back(cacheHit);
        m_y.push_back(++m_time);

        if(m_x.size() >= MAX_POINTS_COUNT) {
            m_x.removeFirst();
            m_y.removeFirst();
        }

        m_cacheHit->graph(0)->setData(m_x, m_x);
        m_cacheHit->xAxis->setRange(m_x.first(), m_x.last());

        double max = *std::max_element(m_y.begin(), m_y.end());
        m_cacheHit->xAxis->setRange(m_y.first(), max);

        m_cacheHit->replot();
    }
}
