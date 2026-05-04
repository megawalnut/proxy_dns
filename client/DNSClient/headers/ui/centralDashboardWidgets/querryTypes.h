#ifndef QUERRYTYPES_H
#define QUERRYTYPES_H

#include <QWidget>
#include <QGridLayout>

#include "dnsTypeCardWidget.h"
#include "../../headers/themes/darkTheme.h"

class QuerryTypes : public QWidget {
public:
    explicit QuerryTypes(QWidget* parent = nullptr);
    void update(const std::vector<CardType::DNSTypeStat>& types);

private:
    void init();

private:
    QGridLayout* m_types = nullptr;
    std::vector<CardType*> m_vectorTypes;
};

#endif // QUERRYTYPES_H
