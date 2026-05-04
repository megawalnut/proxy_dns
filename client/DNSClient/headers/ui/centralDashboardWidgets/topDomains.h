#ifndef TOPDOMAINS_H
#define TOPDOMAINS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

// delete
#include <QStandardItemModel>

#include "../../headers/themes/darkTheme.h"
#include "../../models/domains.h"

class TopDomains : public QWidget {
public:
    explicit TopDomains(QWidget* parent = nullptr);

private:
    void init();

private:
    QVBoxLayout* m_layout = nullptr;
    QTableView* m_domains = nullptr;
};

#endif // TOPDOMAINS_H
