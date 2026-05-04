#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>

#include "../headers/ui/toolBar.h"
#include "../headers/ui/statusBar.h"
#include "../headers/ui/centralWidget.h"


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void init();

private:
    // toolBar
    ToolBar* m_toolBar = nullptr;

    // centralWidget
    CentralWidget* m_centralWidget = nullptr;

    // statusBar
    StatusBar* m_statusBar = nullptr;
};
#endif // MAINWINDOW_H
