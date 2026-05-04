#include "../../headers/ui/toolBar.h"

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
        m_uptime = new QLabel("uptime 01:02:12");
        m_uptime->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_status = new QLabel("🟢 running");
        m_status->setStyleSheet(QString("color: %1;").arg(ACCENT_GREEN.name()));

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
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_rps = new QLabel("127");
        m_rps->setStyleSheet(QString("color: %1;").arg(ACCENT_GREEN.name()));

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
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_cacheBusy= new QLabel("87%");
        m_cacheBusy->setStyleSheet(QString("color: %1;").arg(ACCENT_GREEN.name()));

        cache->addWidget(desc);
        cache->addWidget(m_cacheBusy);
        cache->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ---------------------- Latency -------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* lat = new QVBoxLayout(container);
        QLabel* desc = new QLabel("latency p95");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_latency = new QLabel("8 ms");

        lat->addWidget(desc);
        lat->addWidget(m_latency);
        lat->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ---------------------- Res Avg -------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* avg = new QVBoxLayout(container);
        QLabel* desc = new QLabel("resolve avg");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_resAvg = new QLabel("42 ms");

        avg->addWidget(desc);
        avg->addWidget(m_resAvg);
        avg->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }

    // ---------------------- Errors --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* err = new QVBoxLayout(container);
        QLabel* desc = new QLabel("errors");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_percent = new QLabel("0.4 %");
        m_percent->setStyleSheet(QString("color: %1;").arg(ACCENT_RED.name()));

        err->addWidget(desc);
        err->addWidget(m_percent);
        err->setSpacing(0);

        tbLayout->addWidget(container, 1);
    }
}
