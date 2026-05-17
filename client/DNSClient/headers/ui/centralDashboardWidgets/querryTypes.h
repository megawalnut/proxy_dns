#ifndef QUERRYTYPES_H
#define QUERRYTYPES_H

#include <QWidget>
#include <QGridLayout>
#include <QColor>

#include "../../headers/common/utils.h"
#include "../../headers/basicTypes/dnsTypeCardWidget.h"

using namespace Utils;

class QuerryTypes : public QWidget {
    static constexpr inline std::size_t QUERY_SIZE = 4;
public:
    explicit QuerryTypes(QWidget* parent = nullptr);
    void updateState(const std::vector<MetricRecords::QueryTypeRecord>& types);

private:
    void init();

private:
    QGridLayout* m_types = nullptr;
    std::vector<CardType*> m_vectorTypes;
    std::vector<QColor> m_colors;
};

#endif // QUERRYTYPES_H
