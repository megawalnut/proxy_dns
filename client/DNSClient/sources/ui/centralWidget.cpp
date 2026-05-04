#include "../../headers/ui/centralWidget.h"

CentralWidget::CentralWidget(QWidget* parent) : QWidget(parent) {
    init();
}

void CentralWidget::init() {
    m_grid = new QGridLayout(this);
    m_grid->setContentsMargins(8, 8, 8, 8);
    m_grid->setSpacing(4);

    m_request = new RequestsChart(this);
    m_cacheHit = new CacheHit(this);
    m_p95 = new Latency(this);
    m_domains = new TopDomains(this);
    m_types = new QuerryTypes(this);
    m_errors = new Errors(this);


    m_grid->addWidget(m_request, 0, 0);
    m_grid->addWidget(m_cacheHit, 0, 1);
    m_grid->addWidget(m_p95, 0, 2);
    m_grid->addWidget(m_domains, 1, 0);
    m_grid->addWidget(m_types, 1, 1);
    m_grid->addWidget(m_errors, 1, 2);

    for (int i = 0; i < 3; i++) {
        m_grid->setColumnStretch(i, 1);
    }
    for (int i = 0; i < 2; i++) {
        m_grid->setRowStretch(i, 1);
    }
}
