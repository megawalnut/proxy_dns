#include "../../headers/ui/centralDashboardWidgets/topDomains.h"
#include "../../headers/themes/darkTheme.h"

TopDomains::TopDomains(QWidget* parent) : QWidget(parent) {
    init();
}

void TopDomains::init() {
    // ------------------------ Begin --------------------------
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0,0,0,0);

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

    // ------------------------ Final --------------------------
    m_layout->addWidget(m_view);
    setLayout(m_layout);
}


void TopDomains::updateState(const std::vector<MetricRecords::TopDomainRecord>& domains) {
    int newRows = domains.size();

    m_model->setRowCount(newRows);

    for(int i = 0; i < newRows; ++i) {
        m_model->setData(m_model->index(i, 0), QString::fromStdString(domains[i].domain));
        m_model->setData(m_model->index(i, 1), QString("%1%").arg(domains[i].percent, 0, 'f', 1));
        m_model->setData(m_model->index(i, 2), QString::number(domains[i].requests));

        m_model->setData(m_model->index(i, 0), ACCENT_WHITE, Qt::ForegroundRole);
        m_model->setData(m_model->index(i, 1), ACCENT_GRAY, Qt::ForegroundRole);
        m_model->setData(m_model->index(i, 2), ACCENT_GRAY, Qt::ForegroundRole);
    }
}
