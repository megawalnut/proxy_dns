#ifndef DNSTYPECARDWIDGET_H
#define DNSTYPECARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include "../../headers/themes/darkTheme.h"

class CardType : public QWidget {
public:
    struct DNSTypeStat {
        QString title;
        int percent = 0;
        QColor col{138, 138, 149};
    };

    explicit CardType(QWidget* parent = nullptr);
    void setData(const DNSTypeStat& stat);
    void clear();

private:
    void init();

private:
    QLabel* m_title = nullptr;
    QLabel* m_percent = nullptr;
    QProgressBar* m_progress = nullptr;
};

#endif // DNSTYPECARDWIDGET_H
