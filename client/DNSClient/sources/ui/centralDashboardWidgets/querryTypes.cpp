#include "../../headers/ui/centralDashboardWidgets/querryTypes.h"
#include "../../headers/themes/darkTheme.h"
#include "../../headers/common/utils.h"

QuerryTypes::QuerryTypes(QWidget* parent) : QWidget(parent) {
    init();

    m_colors.reserve(QUERY_SIZE);
    m_colors.emplace_back(ACCENT_BLUE);
    m_colors.emplace_back(ACCENT_GREEN);
    m_colors.emplace_back(ACCENT_AMBER);
    m_colors.emplace_back(ACCENT_RED);
}

void QuerryTypes::init() {
    // ------------------------ Begin --------------------------
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ------------------------- Stack --------------------------
    m_stack = new QStackedWidget(this);
    m_stack->setAttribute(Qt::WA_TranslucentBackground);

    // ------------------------- Cards --------------------------
    QWidget* cardsWidget = new QWidget(m_stack);
    m_types = new QGridLayout(cardsWidget);
    m_types->setContentsMargins(0, 0, 0, 0);
    m_types->setSpacing(4);

    m_vectorTypes.reserve(QUERY_SIZE);
    for(int i = 0; i < QUERY_SIZE; ++i) {
        auto* card = new CardType(cardsWidget);
        card->hide();
        m_vectorTypes.emplace_back(card);
        m_types->addWidget(card, i / 2, i % 2);
    }

    // ------------------------- Empty --------------------------
    m_empty = new QLabel("No data", m_stack);
    m_empty->setAlignment(Qt::AlignCenter);
    m_empty->setStyleSheet(QString(R"(
        QLabel {
            color: %1;
            font-size: 14px;
            background-color: %2;
            border-radius: 10px;
            border: 1px solid %3;
        }
    )").arg(ACCENT_RED.name(), BG_CARD.name(), BORDER.name()));

    // ------------------------ Final --------------------------
    m_stack->addWidget(cardsWidget);
    m_stack->addWidget(m_empty);
    m_stack->setCurrentIndex(1);

    mainLayout->addWidget(m_stack);
}

void QuerryTypes::updateState(const std::vector<MetricRecords::QueryTypeRecord>& types) {
    std::size_t minSize = std::min(types.size(), QUERY_SIZE);
    m_stack->setCurrentIndex(types.empty() ? 1 : 0);

    for(int i = 0; i < QUERY_SIZE; ++i) {
        if(i < minSize) {
            CardType::DNSTypeStat type;
            type.title = QString::fromStdString(types[i].type);
            type.percent = types[i].percent;
            type.col = m_colors[i];
            m_vectorTypes[i]->setData(type);
            m_vectorTypes[i]->show();
        } else {
            m_vectorTypes[i]->hide();
        }
    }
}

void QuerryTypes::disableUI() {
    for(int i = 0; i < QUERY_SIZE; ++i) {
        m_vectorTypes[i]->hide();
    }
    m_stack->setCurrentIndex(1);
}
