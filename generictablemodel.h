#ifndef GENERICTABLEMODEL_H
#define GENERICTABLEMODEL_H

#include <QtSql>


class GenericTableModel : public QSqlRelationalTableModel
{
public:
    GenericTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // GENERICTABLEMODEL_H
