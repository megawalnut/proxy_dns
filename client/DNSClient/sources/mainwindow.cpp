#include "../headers/mainwindow.h"

MainWindow::MainWindow(DashboardController* ctrl, QWidget *parent)
    :
    QMainWindow(parent),
    m_controller(ctrl)
{
    init();
    setupConnection();
}

void MainWindow::onStartUI() {
    show();
}

void MainWindow::onServerUnavailable() {
    m_toolBar->setStatus(false);
    m_toolBar->setUptime({});
}

void MainWindow::init() {
    // ----------------------- ToolBar -------------------------
    m_toolBar = new ToolBar(this);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->addWidget(m_toolBar);
    addToolBar(Qt::TopToolBarArea, toolBar);

    // -------------------- CentralWidget ----------------------
    m_centralWidget = new CentralWidget(this);
    setCentralWidget(m_centralWidget);

    // ---------------------- StatusBar -------------------------
    m_statusBar = new StatusBar(this);
    setStatusBar(m_statusBar);
}

void MainWindow::setupConnection() {
    connect(m_controller, &DashboardController::startUI,
            this, &MainWindow::onStartUI);

    connect(m_controller, &DashboardController::serverUnavailable,
            this, &MainWindow::onServerUnavailable);

    connect(m_controller, &DashboardController::toolBarUpdate,
            this, &MainWindow::onToolBarUpdate);

    connect(m_controller, &DashboardController::centralDataUpdate,
            this, &MainWindow::onCentralDataUpdate);

    connect(m_controller, &DashboardController::statusBarUpdate,
            this, &MainWindow::onStatusBarUpdate);
}

void MainWindow::onToolBarUpdate(Areas::ToolBarData td) {
    m_toolBar->updateState(td);
}
void MainWindow::onCentralDataUpdate(Areas::CentralData cd) {
    m_centralWidget->updateState(cd);
}
void MainWindow::onStatusBarUpdate(Areas::StatusBarData sd) {
    m_statusBar->updateState(sd);
}
