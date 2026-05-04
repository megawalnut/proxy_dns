#include "../../headers/ui/statusBar.h"

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
    init();
}

void StatusBar::init() {
    // ------------------------ Begin --------------------------
    setContentsMargins(5, 0, 5, 0);

    // ------------------------ Cache --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* cache = new QVBoxLayout(container);
        QLabel* desc = new QLabel("cache entries");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_cacheEntries = new QLabel("12 312");

        cache->addWidget(desc);
        cache->addWidget(m_cacheEntries);
        cache->setSpacing(0);

        addWidget(container, 1);
    }
    // ----------------------- Tot Req -------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* total = new QVBoxLayout(container);
        QLabel* desc = new QLabel("total requests");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_requests = new QLabel("4 123 012");

        total->addWidget(desc);
        total->addWidget(m_requests);
        total->setSpacing(0);

        addWidget(container, 1);
    }

    // ------------------------- Ram ---------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* memory = new QVBoxLayout(container);
        QLabel* desc = new QLabel("ram");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_ram= new QLabel("128 MB");

        memory->addWidget(desc);
        memory->addWidget(m_ram);
        memory->setSpacing(0);

        addWidget(container, 1);
    }

    // ----------------------- Cpu --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* proc = new QVBoxLayout(container);
        QLabel* desc = new QLabel("cpu");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_cpu = new QLabel("23 %");

        proc->addWidget(desc);
        proc->addWidget(m_cpu);
        proc->setSpacing(0);

        addWidget(container, 1);
    }

    // ---------------------- Threads -------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* multi = new QVBoxLayout(container);
        QLabel* desc = new QLabel("threads");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_threads = new QLabel("4 active");

        multi->addWidget(desc);
        multi->addWidget(m_threads);
        multi->setSpacing(0);

        addWidget(container, 1);
    }

    // ----------------------- Logo ---------------------------
    {
        m_logo = new QSvgWidget(this);
        m_logo->load(QString("../../dependencies/icons/logo/logo.svg"));
        m_logo->setFixedSize(120, 40);

        addPermanentWidget(m_logo);
    }
}

