#ifndef DNSTYPECARDWIDGET_H
#define DNSTYPECARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include "../themes/darkTheme.h"

class CardType : public QWidget {
public:
    struct DNSTypeStat {
        QString title = "";
        int percent = 0;
        QColor col{ACCENT_GRAY};
    };

    explicit CardType(QWidget* parent = nullptr);
    void setData(const DNSTypeStat& stat);
    void clear();

private:
    void init();

private:
    QColor m_lastColor = nullptr;
    QLabel* m_title = nullptr;
    QLabel* m_percent = nullptr;
    QProgressBar* m_progress = nullptr;
};

#endif // DNSTYPECARDWIDGET_H
