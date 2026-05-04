#ifndef CACHEHITRATE_H
#define CACHEHITRATE_H

#include <QWidget>
#include "../../../dependencies/qcustomplot/qcustomplot.h"
#include "../../../headers/themes/darkTheme.h"

class CacheHit : public QWidget {
public:
    explicit CacheHit(QWidget* parent = nullptr);

private:
    void init();

private:
    QCPTextElement* m_title = nullptr;
    QCustomPlot* m_cacheHit = nullptr;
};

#endif // CACHEHITRATE_H
