#include "../../headers/ui/centralDashboardWidgets/topDomains.h"

TopDomains::TopDomains(QWidget* parent) : QWidget(parent) {
    init();
}

void TopDomains::init() {
    // ------------------------ Begin --------------------------
    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins(0,0,0,0);

    // ------------------------ Table --------------------------
    m_domains = new QTableView(this);

    // ------------------------ Model --------------------------
    //m_model = new Domains();
    //delete this
    auto* model = new QStandardItemModel(3, 3, this);

    model->setHorizontalHeaderLabels({"Domain", "Progress", "Requests"});
    m_domains->horizontalHeader()->setVisible(false);

    auto makeItem = [](const QString& text, const QColor& color = QColor())
    {
        auto* item = new QStandardItem(text);

        if (color.isValid()) {
            item->setForeground(QBrush(color));
        }

        return item;
    };

    model->setItem(0, 0, makeItem("example.com"));
    model->setItem(0, 1, makeItem("100%"));
    model->setItem(0, 2, makeItem("15 329"));

    model->setItem(1, 0, makeItem("google.com"));
    model->setItem(1, 1, makeItem("66%"));
    model->setItem(1, 2, makeItem("10 123"));

    model->setItem(2, 0, makeItem("api.local"));
    model->setItem(2, 1, makeItem("36%"));
    model->setItem(2, 2, makeItem("5 731"));

    model->setItem(3, 0, makeItem("yandex.ru"));
    model->setItem(3, 1, makeItem("14%"));
    model->setItem(3, 2, makeItem("2 272"));

    model->setItem(4, 0, makeItem("pnzgu.ru"));
    model->setItem(4, 1, makeItem("6%"));
    model->setItem(4, 2, makeItem("1 001"));

    // ------------------------ View ---------------------------
    m_domains->setModel(model);
    m_domains->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_domains->setSelectionMode(QAbstractItemView::SingleSelection);
    m_domains->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_domains->verticalHeader()->setVisible(false);
    m_domains->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_domains->setShowGrid(false);
    m_domains->setFocusPolicy(Qt::NoFocus);

    // ------------------------ Style --------------------------
    m_domains->setStyleSheet(QString(R"(
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
    m_layout->addWidget(m_domains);
    setLayout(m_layout);
}
