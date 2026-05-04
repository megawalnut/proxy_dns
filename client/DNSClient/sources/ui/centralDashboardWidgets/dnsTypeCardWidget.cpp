#include "../../headers/ui/centralDashboardWidgets/dnsTypeCardWidget.h"

CardType::CardType(QWidget* parent) : QWidget(parent) {
    init();
}

void CardType::init() {
    // ------------------------ Begin --------------------------
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(2);

    // ------------------------ Title --------------------------
    {
        m_title = new QLabel();
        layout->addWidget(m_title);
    }

    // ----------------------- Percent -------------------------
    {
        m_percent = new QLabel();
        m_percent->setStyleSheet(QString("font-size: 16px; color: %1").arg(ACCENT_WHITE.name()));
        layout->addWidget(m_percent);
    }

    // ----------------------- Progress ------------------------
    {
        m_progress = new QProgressBar();
        m_progress->setRange(0, 100);
        m_progress->setTextVisible(false);
        m_progress->setFixedHeight(6);
        layout->addWidget(m_progress);
    }

    // ------------------------ View --------------------------
    setObjectName("chartCard");
    setStyleSheet(QString(R"(
        QWidget#chartCard {
            background-color: %1;
            border-radius: 10px;
            border: 1px solid %2;
        }
    )").arg(BG_CARD.name(), BORDER.name()));
}

void CardType::setData(const DNSTypeStat& stat) {
    m_title->setText(stat.title);
    m_title->setStyleSheet(QString(R"(
        QLabel {
            font-size: 12px;
            color: %1;
        }
    )").arg(stat.col.name()));

    m_percent->setText(QString("%1%").arg(stat.percent));

    m_progress->setValue(stat.percent);
    m_progress->setStyleSheet(QString(R"(
        QProgressBar {
            background-color: %1;
            border-radius: 6px;
        }

        QProgressBar::chunk {
            background-color: %2;
            border-radius: 3px;
        }
    )").arg(BG_CARD.name(), stat.col.name()));
}

// unused
void CardType::clear() {
    m_title->setText("");
    m_percent->setText("");
    m_progress->setValue(0);
}
