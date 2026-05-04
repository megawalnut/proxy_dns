#ifndef RECENTERRORS_H
#define RECENTERRORS_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>

// delete
#include <QStandardItemModel>

#include "../../headers/themes/darkTheme.h"
#include "../../models/errorsModel.h"

class Errors : public QWidget {
public:
    explicit Errors(QWidget* parent = nullptr);

private:
    void init();

private:
    QVBoxLayout* m_layout = nullptr;
    QTableView* m_view = nullptr;
    ErrorsModel* m_model = nullptr;
    QWidget* m_errors = nullptr;
};

#endif // RECENTERRORS_H




