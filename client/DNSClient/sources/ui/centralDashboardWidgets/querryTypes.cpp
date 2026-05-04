#include "../../headers/ui/centralDashboardWidgets/querryTypes.h"

QuerryTypes::QuerryTypes(QWidget* parent) : QWidget(parent) {
    init();
}

void QuerryTypes::init() {
    // ------------------------ Begin --------------------------
    m_types = new QGridLayout(this);
    m_types->setContentsMargins(0, 0, 0, 0);
    m_types->setSpacing(4);

    m_vectorTypes.reserve(4);
    for(int i = 0; i < 4; ++i) {
        m_vectorTypes.emplace_back(new CardType(this));
    }

    // ------------------------ Views --------------------------
    // delete this
    m_types->addWidget(m_vectorTypes[0], 0, 0);
    m_types->addWidget(m_vectorTypes[1], 0, 1);
    m_types->addWidget(m_vectorTypes[2], 1, 0);
    m_types->addWidget(m_vectorTypes[3], 1, 1);

    std::vector<CardType::DNSTypeStat> vec = {{"A", 68, ACCENT_BLUE},
                                              {"AAAA", 23, ACCENT_GREEN},
                                              {"MX", 5, ACCENT_AMBER},
                                              {"NS", 4, ACCENT_RED}};

    // ------------------------ Final --------------------------
    update(vec);
}

void QuerryTypes::update(const std::vector<CardType::DNSTypeStat>& types) {
    std::size_t minSize = std::min(types.size(), m_vectorTypes.size());

    for(int i = 0; i < minSize; ++i) {
        m_vectorTypes[i]->setData(types[i]);
    }
}
