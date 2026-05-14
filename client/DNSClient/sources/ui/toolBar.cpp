#include "../../headers/ui/toolBar.h"
#include "../../headers/themes/darkTheme.h"

ToolBar::ToolBar(QWidget* parent) : QWidget(parent) {
    init();
}

void ToolBar::init() {
    // ------------------------ Begin --------------------------
    QHBoxLayout* tbLayout = new QHBoxLayout(this);
    tbLayout->setContentsMargins(5, 0, 5, 0);
    tbLayout->setSpacing(4);
    tbLayout->setAlignment(Qt::AlignVCenter);

    // ------------------------ State --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* state = new QVBoxLayout(container);
        m_uptime = new QLabel();
        m_status = new QLabel();
        m_uptime->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_status->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));

        state->addWidget(m_uptime);
        state->addWidget(m_status);
        state->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ------------------------- RPS ---------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* request = new QVBoxLayout(container);
        QLabel* desc = new QLabel("request / sec");
        m_rps = new QLabel();
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_rps->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));

        request->addWidget(desc);
        request->addWidget(m_rps);
        request->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ------------------------ Cache --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* cache = new QVBoxLayout(container);
        QLabel* desc = new QLabel("cache hit rate");
        m_cacheHits = new QLabel();
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_cacheHits->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));

        cache->addWidget(desc);
        cache->addWidget(m_cacheHits);
        cache->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ---------------------- Latency --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* lat = new QVBoxLayout(container);
        QLabel* desc = new QLabel("latency p95");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_latency = new QLabel();

        lat->addWidget(desc);
        lat->addWidget(m_latency);
        lat->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ---------------------- Res Avg --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* avg = new QVBoxLayout(container);
        QLabel* desc = new QLabel("resolve avg");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_resAvg = new QLabel();

        avg->addWidget(desc);
        avg->addWidget(m_resAvg);
        avg->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ---------------------- Errors ---------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* err = new QVBoxLayout(container);
        QLabel* desc = new QLabel("errors");
        m_errors = new QLabel();
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_errors->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));

        err->addWidget(desc);
        err->addWidget(m_errors);
        err->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }
}

void ToolBar::updateState(Utils::Areas::ToolBarData td) {
    setStatus(td.running);
    setUptime(td.uptime_sec);   // ms
    setRPC(td.requests_sec);
    setCacheHit(td.cache_hit);
    setLatency(td.latency_p95);
    setResolveAvg(td.resolve_avg);
    setErrorsPercent(td.errors);
}

void ToolBar::disableUI() {
    setStatus({});
    setUptime({});
    setRPC({});
    setCacheHit({});
    setLatency({});
    setResolveAvg({});
    setErrorsPercent({});
}

void ToolBar::setStatus(bool state) {
    m_status->setText(state ? "🟢 running"
                            : "🔴 stopped");
    m_status->setStyleSheet(QString("color: %1;").arg(state ? ACCENT_GREEN.name()
                                                            : ACCENT_RED.name()));
}
void ToolBar::setUptime(double msec) {
    QTime t(0, 0, 0);
    QTime n;
    n = t.addMSecs(msec);
    QString str = QString("uptime %1").arg(n.toString("hh:mm:ss"));
    m_uptime->setText(str);
}
void ToolBar::setRPC(uint32_t rpc) {
    m_rps->setText(QString("%1").arg(QString::number(rpc, 10)));
    if(rpc <= 50) {
        m_rps->setStyleSheet(QString("color: %1;").arg(ACCENT_GREEN.name()));
    } else if(rpc > 50 && rpc <= 150) {
        m_rps->setStyleSheet(QString("color: %1;").arg(ACCENT_AMBER.name()));
    } else {
        m_rps->setStyleSheet(QString("color: %1;").arg(ACCENT_RED.name()));
    }
}
void ToolBar::setCacheHit(double hits) {
    m_cacheHits->setText(QString("%1 %").arg(hits, 0, 'f', 1));
    if(hits >= 80) {
        m_cacheHits->setStyleSheet(QString("color: %1;").arg(ACCENT_GREEN.name()));
    } else if(hits >= 50 && hits < 80) {
        m_cacheHits->setStyleSheet(QString("color: %1;").arg(ACCENT_AMBER.name()));
    } else {
        m_cacheHits->setStyleSheet(QString("color: %1;").arg(ACCENT_RED.name()));
    }
}
void ToolBar::setLatency(double p95) {
    m_latency->setText(QString("%1 ms").arg(p95, 0, 'f', 1));
}
void ToolBar::setResolveAvg(double resAvg) {
    m_resAvg->setText(QString("%1 ms").arg(resAvg, 0, 'f', 1));
}
void ToolBar::setErrorsPercent(double errs) {
    m_errors->setText(QString("%1 %").arg(errs, 0, 'f', 1));
    if(errs <= 1) {
        m_errors->setStyleSheet(QString("color: %1;").arg(ACCENT_GREEN.name()));
    } else if(errs > 1 && errs < 5) {
        m_errors->setStyleSheet(QString("color: %1;").arg(ACCENT_AMBER.name()));
    } else {
        m_errors->setStyleSheet(QString("color: %1;").arg(ACCENT_RED.name()));
    }
}
