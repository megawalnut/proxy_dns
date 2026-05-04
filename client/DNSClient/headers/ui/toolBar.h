#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "../../headers/themes/darkTheme.h"

class ToolBar : public QWidget {
    Q_OBJECT
public:
    explicit ToolBar(QWidget* parent = nullptr);

private:
    void init();

private:
    QLabel* m_status = nullptr;
    QLabel* m_uptime = nullptr;
    QLabel* m_rps = nullptr;
    QLabel* m_cacheBusy = nullptr;
    QLabel* m_latency = nullptr;
    QLabel* m_resAvg = nullptr;
    QLabel* m_percent = nullptr;
};
#endif // TOOLBAR_H
