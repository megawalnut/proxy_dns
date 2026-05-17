#ifndef TOPDOMAINS_H
#define TOPDOMAINS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QLabel>

#include "../../headers/common/utils.h"

using namespace Utils;

class TopDomains : public QWidget {
public:
    explicit TopDomains(QWidget* parent = nullptr);
    void updateState(const std::vector<MetricRecords::TopDomainRecord>& domains);
    void disableUI();

private:
    void init();

private:
    QStackedWidget* m_stack = nullptr;
    QLabel* m_empty = nullptr;
    QStandardItemModel* m_model = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QTableView* m_view = nullptr;
};

#endif // TOPDOMAINS_H
