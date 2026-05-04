#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QVBoxLayout>
#include <QLabel>
#include <QSvgWidget>
#include <QStatusBar>

#include "../../headers/themes/darkTheme.h"

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = nullptr);

private:
    void init();

private:
    QLabel* m_cacheEntries = nullptr;
    QLabel* m_requests = nullptr;
    QLabel* m_ram = nullptr;
    QLabel* m_cpu = nullptr;
    QLabel* m_threads = nullptr;
    QSvgWidget* m_logo = nullptr;
};
#endif // STATUSBAR_H
