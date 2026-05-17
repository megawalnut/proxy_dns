#ifndef RECENTERRORS_H
#define RECENTERRORS_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDateTime>

#include "../../headers/common/utils.h"

using namespace Utils;

class Errors : public QWidget {
public:
    explicit Errors(QWidget* parent = nullptr);
    void updateState(const std::vector<MetricRecords::ErrorRecord>& errors);

private:
    void init();
    static QColor getErrColor(const std::string& str);

private:
    QVBoxLayout* m_layout = nullptr;
    QTableView* m_view = nullptr;
    QStandardItemModel* m_model = nullptr;
    QWidget* m_errors = nullptr;
};

#endif // RECENTERRORS_H




