#include "../../headers/ui/centralWidget.h"

CentralWidget::CentralWidget(QWidget* parent) : QWidget(parent) {
    init();
}

void CentralWidget::init() {
    m_grid = new QGridLayout(this);
    m_grid->setContentsMargins(8, 8, 8, 8);
    m_grid->setSpacing(4);

    m_request = new BasePlot("requests / sec — last 60s", ACCENT_BLUE, ACCENT_BLUE_BRUSH, this);
    m_cacheHit = new BasePlot("cache hit rate — last 60s", ACCENT_GREEN, ACCENT_GREEN_BRUSH, this);
    m_p95 = new BasePlot("latency p95 ms — last 60s", ACCENT_AMBER, ACCENT_AMBER_BRUSH, this);
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

void CentralWidget::updateState(Utils::Areas::CentralData cd) {
    m_request->updateState(cd.requests_sec);
    m_cacheHit->updateState(cd.cache_hit);
    m_p95->updateState(cd.latency_p95);
    m_domains->updateState(cd.top_domains);
    m_types->updateState(cd.query_types);
    m_errors->updateState(cd.recent_errors);
}
