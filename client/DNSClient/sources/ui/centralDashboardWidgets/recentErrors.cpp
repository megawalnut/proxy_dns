#include "../../headers/ui/centralDashboardWidgets/recentErrors.h"
#include "../../headers/themes/darkTheme.h"

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

    // ------------------------ Final --------------------------
    m_layout->addWidget(m_view);
    setLayout(m_layout);
}

void Errors::updateState(const std::vector<MetricRecords::ErrorRecord>& errors) {
    int newRows = errors.size();

    m_model->setRowCount(newRows);

    for(int i = 0; i < newRows; ++i) {
        QString timeStr = errors[i].time > 0
                              ? QDateTime::fromMSecsSinceEpoch((qint64)errors[i].time).toString("hh:mm:ss")
                              : "-";

        m_model->setData(m_model->index(i, 0), timeStr);
        m_model->setData(m_model->index(i, 1), QString::fromStdString(errors[i].domain));
        m_model->setData(m_model->index(i, 2), QString::fromStdString(errors[i].error));

        m_model->setData(m_model->index(i, 0), ACCENT_GRAY, Qt::ForegroundRole);
        m_model->setData(m_model->index(i, 1), ACCENT_WHITE,Qt::ForegroundRole);
        m_model->setData(m_model->index(i, 2), getErrColor(errors[i].error), Qt::ForegroundRole);
    }
}

/*static*/
QColor Errors::getErrColor(const std::string& str) {
    if (str == "TIMEOUT")  return ACCENT_AMBER;
    if (str == "NXDOMAIN") return ACCENT_RED;
    if (str == "SERVFAIL") return ACCENT_RED;
    return ACCENT_GRAY;
}
