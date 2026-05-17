#ifndef RECENTERRORS_H
#define RECENTERRORS_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDateTime>
#include <QStackedWidget>
#include <QLabel>

#include "../../headers/common/utils.h"

using namespace Utils;

class Errors : public QWidget {
public:
    explicit Errors(QWidget* parent = nullptr);
    void updateState(const std::vector<MetricRecords::ErrorRecord>& errors);
    void disableUI();

private:
    void init();
    static QColor getErrColor(const std::string& str);

private:
    QStackedWidget* m_stack = nullptr;
    QLabel* m_empty = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QTableView* m_view = nullptr;
    QStandardItemModel* m_model = nullptr;
    QWidget* m_errors = nullptr;
};

#endif // RECENTERRORS_H




