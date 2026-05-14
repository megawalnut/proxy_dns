#include "../../headers/ui/statusBar.h"
#include "../../headers/themes/darkTheme.h"

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
        m_cacheEntries = new QLabel();

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
        m_requests = new QLabel();

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
        m_ram= new QLabel();

        memory->addWidget(desc);
        memory->addWidget(m_ram);
        memory->setSpacing(0);

        addWidget(container, 1);
    }

    // ----------------------- Cpu -----------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* proc = new QVBoxLayout(container);
        QLabel* desc = new QLabel("cpu");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_cpu = new QLabel();

        proc->addWidget(desc);
        proc->addWidget(m_cpu);
        proc->setSpacing(0);

        addWidget(container, 1);
    }

    // ---------------------- Threads --------------------------
    {
        QWidget* container = new QWidget();
        QVBoxLayout* multi = new QVBoxLayout(container);
        QLabel* desc = new QLabel("threads");
        desc->setStyleSheet(QString("color: %1;").arg(ACCENT_GRAY.name()));
        m_threads = new QLabel();

        multi->addWidget(desc);
        multi->addWidget(m_threads);
        multi->setSpacing(0);

        addWidget(container, 1);
    }

    // ----------------------- Logo ----------------------------
    {
        m_logo = new QSvgWidget(this);
        m_logo->load(QString("../../dependencies/icons/logo/logo.svg"));
        m_logo->setFixedSize(120, 40);

        addPermanentWidget(m_logo);
    }
}

void StatusBar::updateState(Utils::Areas::StatusBarData sd) {
    setCacheEntries(sd.cache_entries);
    setTotalRequests(sd.total_requests);
    setRam(sd.ram);
    setCpu(sd.cpu);
    setThreads(sd.threads);
}

void StatusBar::disableUI() {
    setCacheEntries({});
    setTotalRequests({});
    setRam({});
    setCpu({});
    setThreads({});
}

void StatusBar::setCacheEntries(uint64_t entries) {
    m_cacheEntries->setText(QString::number(entries, 10));
}
void StatusBar::setTotalRequests(uint64_t requests) {
    m_requests->setText(QString::number(requests, 10));
}
void StatusBar::setRam(uint64_t ram) {
    m_ram->setText(QString("%1 MB").arg(QString::number(ram, 10)));
}
void StatusBar::setCpu(double cpu) {
    m_cpu->setText(QString("%1 %").arg(cpu, 0, 'f', 1));
}
void StatusBar::setThreads(uint32_t threads) {
    m_threads->setText(QString("%1 active").arg(QString::number(threads, 10)));
}

