#include "generictablemodel.h"

GenericTableModel::GenericTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{

}

QVariant GenericTableModel::data(const QModelIndex &index, int role) const
{
    // Format Date and Time
    if (role == Qt::DisplayRole){
        if(index.column() == fieldIndex("date_ordered") ||
           index.column() == fieldIndex("date_created"))
            return QVariant(qvariant_cast<QDateTime>(QSqlTableModel::data(index, role)).toString(Qt::SystemLocaleShortDate));
    }
    return QSqlTableModel::data(index, role);
}
