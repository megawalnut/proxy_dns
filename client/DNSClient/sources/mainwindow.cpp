#include "../headers/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    //setMinimumSize(540, 400);
    init();
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
