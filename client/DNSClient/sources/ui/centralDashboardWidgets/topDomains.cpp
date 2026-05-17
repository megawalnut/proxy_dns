#include "../../headers/ui/centralDashboardWidgets/topDomains.h"
#include "../../headers/themes/darkTheme.h"

TopDomains::TopDomains(QWidget* parent) : QWidget(parent) {
    init();
}

void TopDomains::init() {
    // ------------------------ Begin --------------------------
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0,0,0,0);

    // ------------------------- Stack --------------------------
    m_stack = new QStackedWidget(this);
    m_stack->setAttribute(Qt::WA_TranslucentBackground);

    // ------------------------ Table --------------------------
    m_view = new QTableView(this);

    // ------------------------ Model --------------------------
    m_model = new QStandardItemModel(0, 3, this);
    m_view->horizontalHeader()->setVisible(false);
    m_view->setModel(m_model);

    // ------------------------ View ---------------------------
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->verticalHeader()->setVisible(false);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setShowGrid(false);
    m_view->setFocusPolicy(Qt::NoFocus);

    // ------------------------ Style --------------------------
    m_view->setStyleSheet(QString(R"(
        QTableView {
            background-color: %1;
            border-radius: 10px;
            outline: none;
            border: none;
        }

        QTableView::viewport {
            background-color: %2;
        }

        QTableView::item:selected {
            background: rgba(77, 168, 255, 0.4);
            border: none;
        }
    )").arg(BG_CARD.name(), BG_CARD.name()));

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
    m_stack->addWidget(m_view);
    m_stack->addWidget(m_empty);
    m_stack->setCurrentIndex(1);

    m_layout->addWidget(m_stack);
    setLayout(m_layout);
}


void TopDomains::updateState(const std::vector<MetricRecords::TopDomainRecord>& domains) {
    m_model->removeRows(0, m_model->rowCount());

    auto makeItem = [](const QString& text, const QColor& color = ACCENT_GRAY) {
        auto* item = new QStandardItem(text);
        if (color.isValid()) {
            item->setForeground(QBrush(color));
        }
        return item;
    };

    for(const auto& dom : domains) {
        QList<QStandardItem*> row;
        row.append(makeItem(QString::fromStdString(dom.domain), ACCENT_WHITE));
        row.append(makeItem(QString("%1%").arg(dom.percent, 0, 'f', 1)));
        row.append(makeItem(QString::number(dom.requests)));

        m_model->appendRow(row);
    }
    m_stack->setCurrentIndex(domains.empty() ? 1 : 0);
}

void TopDomains::disableUI() {
    m_model->removeRows(0, m_model->rowCount());
    m_stack->setCurrentIndex(1);
}
