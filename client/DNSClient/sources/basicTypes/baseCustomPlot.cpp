#include "../../headers/basicTypes/baseCustomPlot.h"
#include "../../headers/themes/darkTheme.h"

BasePlot::BasePlot(const QString& title, const QColor& col, const QColor& gradient, QWidget* parent) : QWidget(parent) {
    init(title, col, gradient);
}

void BasePlot::init(const QString& title, const QColor& col, const QColor& gradient) {
    // ------------------------ Begin --------------------------
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(0);

    // ------------------------- View ---------------------------
    m_plot = new QCustomPlot(this);

    // ------------------------ Title --------------------------
    m_plot->plotLayout()->insertRow(0);

    m_title = new QCPTextElement(m_plot, title, QFont("Inner", 8));
    m_title->setTextColor(Qt::gray);
    m_title->setTextFlags(Qt::AlignLeft | Qt::AlignVCenter);

    m_plot->plotLayout()->addElement(0, 0, m_title);

    // ------------------------- Graph -------------------------
    // background
    m_plot->setBackground(Qt::transparent);
    m_plot->axisRect()->setBackground(Qt::transparent);

    // padding
    m_plot->axisRect()->setAutoMargins(QCP::msNone);
    m_plot->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // graph
    m_plot->addGraph();

    m_plot->graph(0)->setPen(QPen(col, 1));
    m_plot->graph(0)->setBrush(gradient);

    m_plot->legend->setVisible(false);

    m_plot->xAxis->setVisible(false);
    m_plot->yAxis->setVisible(false);
    m_plot->xAxis->grid()->setVisible(false);
    m_plot->yAxis->grid()->setVisible(false);
    m_plot->setInteractions(QCP::iNone);

    // ------------------------ Style --------------------------
    setObjectName("basePlot");
    setStyleSheet(QString(R"(
        QWidget#basePlot {
            background-color: %1;
            border-radius: 10px;
            border: 1px solid %2;
        }
    )").arg(BG_CARD.name(), BORDER.name()));

    // ------------------------ Final --------------------------
    layout->addWidget(m_plot);
}

void BasePlot::updateState(double lat) {
    m_x.push_back(++m_time);
    if(lat > 0) {
        m_lastValue = lat;
        m_y.push_back(lat);
    }
    else {
        m_y.push_back(m_lastValue);
    }

    if(m_x.size() >= MAX_POINTS_COUNT) {
        m_x.removeFirst();
        m_y.removeFirst();
    }

    m_plot->graph(0)->setData(m_x, m_y);
    m_plot->xAxis->setRange(m_x.first(), m_x.last());

    double max = *std::max_element(m_y.begin(), m_y.end());
    m_plot->yAxis->setRange(0, max > 0 ? max * 1.2 : 10);

    if (!isVisible()) {
        return;
    }

    m_plot->replot(QCustomPlot::rpQueuedReplot);
}
