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
    m_types = new QGridLayout(this);
    m_types->setContentsMargins(0, 0, 0, 0);
    m_types->setSpacing(4);

    // ------------------------ Final --------------------------
    m_vectorTypes.reserve(QUERY_SIZE);
    for(int i = 0; i < QUERY_SIZE; ++i) {
        auto* card = new CardType(this);
        m_vectorTypes.emplace_back(card);
        m_types->addWidget(card, i / 2, i % 2);
    }
}

void QuerryTypes::updateState(const std::vector<MetricRecords::QueryTypeRecord>& types) {
    std::size_t minSize = std::min(types.size(), QUERY_SIZE);

    for(int i = 0; i < QUERY_SIZE; ++i) {
        if (i < minSize) {
            CardType::DNSTypeStat type;
            type.title = QString::fromStdString(types[i].type);
            type.percent = types[i].percent;
            type.col = m_colors[i % m_colors.size()];

            m_vectorTypes[i]->setData(type);
            m_vectorTypes[i]->show();
        } else {
            m_vectorTypes[i]->hide();
        }
    }
}
