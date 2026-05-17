#include "../../headers/ui/centralDashboardWidgets/recentErrors.h"
#include "../../headers/themes/darkTheme.h"

Errors::Errors(QWidget* parent) : QWidget(parent) {
    init();
}

void Errors::init() {
    // ------------------------ Begin --------------------------
    m_layout = new QVBoxLayout();
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
            background: rgba(61, 203, 138, 0.4);
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

void Errors::updateState(const std::vector<MetricRecords::ErrorRecord>& errors) {
    int newRows = errors.size();
    int curRows = m_model->rowCount();

    // add new rows
    while(m_model->rowCount() < newRows) {
        m_model->appendRow({new QStandardItem, new QStandardItem,new QStandardItem});
    }

    // delete excess rows
    if(newRows < curRows) {
        m_model->removeRows(newRows, curRows - newRows);
    }

    for(int i = 0; i < newRows; ++i) {
        if (errors[i].time <= 0) {
            continue;
        }
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(errors[i].time));
        m_model->item(i, 0)->setText(dt.toString("hh:mm:ss"));
        m_model->item(i, 0)->setForeground(ACCENT_GRAY);
        m_model->item(i, 1)->setText(QString::fromStdString(errors[i].domain));
        m_model->item(i, 1)->setForeground(ACCENT_WHITE);
        m_model->item(i, 2)->setText(QString::fromStdString(errors[i].error));
        m_model->item(i, 2)->setForeground(getErrColor(errors[i].error));
    }
    m_stack->setCurrentIndex(errors.empty() ? 1 : 0);
}

void Errors::disableUI() {
    m_model->removeRows(0, m_model->rowCount());
    m_stack->setCurrentIndex(1);
}

/*static*/
QColor Errors::getErrColor(const std::string& str) {
    if (str == "TIMEOUT")  return ACCENT_AMBER;
    if (str == "NXDOMAIN") return ACCENT_RED;
    if (str == "SERVFAIL") return ACCENT_RED;
    return ACCENT_GRAY;
}
