#include "../headers/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    init();
}

void MainWindow::init() {
    m_toolBar = new QToolBar(this);
    m_toolBar->setMinimumHeight(60);
    m_toolBar->setIconSize(QSize(16, 16));
    addToolBar(Qt::TopToolBarArea, m_toolBar);

    // ------------------------ Begin --------------------------
    QWidget* tbContainer = new QWidget(m_toolBar);
    QHBoxLayout* tbLayout = new QHBoxLayout(tbContainer);
    tbLayout->setContentsMargins(5, 0, 5, 0);
    tbLayout->setSpacing(10);
    tbLayout->setAlignment(Qt::AlignVCenter);

    // ------------------------ State --------------------------
    {
        QVBoxLayout* state = new QVBoxLayout();
        QLabel* status = new QLabel("🟢 running");
        status->setStyleSheet("color: green;");
        status->setAlignment(Qt::AlignCenter);
        QLabel* uptime = new QLabel("uptime 01:02:12");
        uptime->setStyleSheet("color: gray;");

        state->addWidget(status);
        state->addWidget(uptime);
        state->setSpacing(0);

        tbLayout->addLayout(state);
    }
    // ------------------------- RPS ---------------------------
    {
        QVBoxLayout* rps = new QVBoxLayout();
        QLabel* desc = new QLabel("request / sec");
        desc->setStyleSheet("color: gray;");
        QLabel* value = new QLabel("127");
        value->setStyleSheet("color: green;");
        value->setAlignment(Qt::AlignCenter);

        rps->addWidget(desc);
        rps->addWidget(value);
        rps->setSpacing(0);

        tbLayout->addLayout(rps);
    }

    // ------------------------ Cache --------------------------
    {
        QVBoxLayout* cache = new QVBoxLayout();
        QLabel* desc = new QLabel("cache hit rate");
        desc->setStyleSheet("color: gray;");
        QLabel* value = new QLabel("87%");
        value->setStyleSheet("color: green;");
        value->setAlignment(Qt::AlignCenter);

        cache->addWidget(desc);
        cache->addWidget(value);
        cache->setSpacing(0);

        tbLayout->addLayout(cache);
    }

    // ---------------------- Latency -------------------------
    {
        QVBoxLayout* lat = new QVBoxLayout();
        QLabel* desc = new QLabel("latency p95");
        desc->setStyleSheet("color: gray;");
        QLabel* value = new QLabel("8 ms");
        value->setAlignment(Qt::AlignCenter);

        lat->addWidget(desc);
        lat->addWidget(value);
        lat->setSpacing(0);

        tbLayout->addLayout(lat);
    }

    // ---------------------- Res Avg -------------------------
    {
        QVBoxLayout* avg = new QVBoxLayout();
        QLabel* desc = new QLabel("resolve avg");
        desc->setStyleSheet("color: gray;");
        QLabel* value = new QLabel("42 ms");
        value->setAlignment(Qt::AlignCenter);

        avg->addWidget(desc);
        avg->addWidget(value);
        avg->setSpacing(0);

        tbLayout->addLayout(avg);
    }

    // ---------------------- Errors --------------------------
    {
        QVBoxLayout* err = new QVBoxLayout();
        QLabel* desc = new QLabel("errors");
        desc->setStyleSheet("color: gray;");
        QLabel* value = new QLabel("0.4 %");
        desc->setStyleSheet("color: red;");
        value->setAlignment(Qt::AlignCenter);

        err->addWidget(desc);
        err->addWidget(value);
        err->setSpacing(0);

        tbLayout->addLayout(err);
    }

    // ------------------------ Final --------------------------
    tbContainer->setLayout(tbLayout);
    m_toolBar->addWidget(tbContainer);


}

MainWindow::~MainWindow() {}
