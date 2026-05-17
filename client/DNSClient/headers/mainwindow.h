#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>

#include "../headers/controller/dashboardController.h"
#include "../headers/common/utils.h"

#include "../headers/ui/toolBar.h"
#include "../headers/ui/statusBar.h"
#include "../headers/ui/centralWidget.h"

using namespace Utils;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(DashboardController* ctrl, QWidget* parent = nullptr);

private:
    void init();
    void setupConnection();

private slots:
    void onStartUI();
    void onServerUnavailable();

    void onToolBarUpdate(Areas::ToolBarData);
    void onCentralDataUpdate(Areas::CentralData);
    void onStatusBarUpdate(Areas::StatusBarData);

private:
    DashboardController* m_controller = nullptr;
    // toolBar
    ToolBar* m_toolBar = nullptr;
    // centralWidget
    CentralWidget* m_centralWidget = nullptr;
    // statusBar
    StatusBar* m_statusBar = nullptr;
};
#endif // MAINWINDOW_H
