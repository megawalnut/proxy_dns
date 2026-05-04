#include "../../headers/ui/centralDashboardWidgets/recentErrors.h"

Errors::Errors(QWidget* parent) : QWidget(parent) {
    init();
}

void Errors::init() {
    // ------------------------ Begin --------------------------
    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins(0,0,0,0);

    // ------------------------ Table --------------------------
    m_view = new QTableView(this);

    // ------------------------ Model --------------------------
    //m_model = new ErrorsModel();
    //delete this
    auto* model = new QStandardItemModel(3, 3, this);
    model->setHorizontalHeaderLabels({"Time", "Domain", "Status"});
    m_view->horizontalHeader()->setVisible(false);

    auto makeItem = [](const QString& text, const QColor& color = QColor())
    {
        auto* item = new QStandardItem(text);

        if (color.isValid()) {
            item->setForeground(QBrush(color));
        }

        return item;
    };

    model->setItem(0, 0, makeItem("01:01:12"));
    model->setItem(0, 1, makeItem("example.com"));
    model->setItem(0, 2, makeItem("NXDOMAIN", ACCENT_RED));

    model->setItem(1, 0, makeItem("01:01:13"));
    model->setItem(1, 1, makeItem("google.com"));
    model->setItem(1, 2, makeItem("SERVFAIL", ACCENT_RED));

    model->setItem(2, 0, makeItem("01:01:14"));
    model->setItem(2, 1, makeItem("api.local"));
    model->setItem(2, 2, makeItem("TIMEOUT", ACCENT_AMBER));

    model->setItem(3, 0, makeItem("01:01:14"));
    model->setItem(3, 1, makeItem("yandex.ru"));
    model->setItem(3, 2, makeItem("SERVFAIL", ACCENT_RED));

    model->setItem(4, 0, makeItem("01:01:14"));
    model->setItem(4, 1, makeItem("pnzgu.ru"));
    model->setItem(4, 2, makeItem("TIMEOUT", ACCENT_AMBER));

    // ------------------------ View ---------------------------
    m_view->setModel(model);
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
            background: rgba(61, 203, 138, 0.4);
            border: none;
        }
    )").arg(BG_CARD.name(), BG_CARD.name()));


    // ------------------------ Final --------------------------
    m_layout->addWidget(m_view);
    setLayout(m_layout);
}
