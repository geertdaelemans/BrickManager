#include "generictablemodel.h"

GenericTableModel::GenericTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{

}

QVariant GenericTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole){
        // Format Date and Time
        if(index.column() == fieldIndex("date_ordered") ||
           index.column() == fieldIndex("date_created"))
            return QVariant(qvariant_cast<QDateTime>(QSqlTableModel::data(index, role)).toString(Qt::SystemLocaleShortDate));

        // Calculate total
        if(index.column() == fieldIndex("Total")) {
            int quantity = data(this->index(index.row(),fieldIndex("Qty"))).toInt();
            double price = data(this->index(index.row(),fieldIndex("Price"))).toDouble();;
            return QVariant(price * quantity);
        }

    }
    return QSqlTableModel::data(index, role);
}
