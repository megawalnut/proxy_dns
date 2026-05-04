#ifndef ERRORSMODEL_H
#define ERRORSMODEL_H

#include <QAbstractTableModel>

class ErrorsModel : public QAbstractTableModel {
public:
    struct ErrorEntry {
        QString time;
        QString domain;
        QString message;
    };

    explicit ErrorsModel(QObject* parent = nullptr);

private:
    void init();

private:
};

#endif // ERRORSMODEL_H





