#ifndef GENERICTABLEMODEL_H
#define GENERICTABLEMODEL_H

#include <QtSql>


class GenericTableModel : public QSqlTableModel
{
public:
    GenericTableModel(QObject * = nullptr, QSqlDatabase = QSqlDatabase());
    QVariant data(const QModelIndex &, int = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &) const override;
};

#endif // GENERICTABLEMODEL_H
