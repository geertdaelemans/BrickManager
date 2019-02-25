#include "generictablemodel.h"

GenericTableModel::GenericTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{

}

Qt::ItemFlags GenericTableModel::flags(const QModelIndex &index) const
{
    // Prohibit certain columns to be edited
    auto flags = QSqlTableModel::flags(index);
    if(index.column() == fieldIndex("id") ||
       index.column() == fieldIndex("total") ||
       index.column() == fieldIndex("orig_qty") ||
       index.column() == fieldIndex("orig_qty_diff") ||
       index.column() == fieldIndex("orig_price") ||
       index.column() == fieldIndex("orig_price_diff") ||
       index.column() == fieldIndex("is_retain") ||
       index.column() == fieldIndex("is_stock_room")) {
       flags &= Qt::ItemIsEditable;
    }
    return flags;
}

QVariant GenericTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole){

        // Format Date and Time
        if(index.column() == fieldIndex("date_ordered") ||
           index.column() == fieldIndex("date_created"))
            return QVariant(qvariant_cast<QDateTime>(QSqlTableModel::data(index, role)).toString("yyyy-MM-dd hh:mm:ss"));

        // Calculate total
        if(index.column() == fieldIndex("total")) {
            int quantity = data(this->index(index.row(),fieldIndex("quantity"))).toInt();
            double price = data(this->index(index.row(),fieldIndex("unit_price"))).toDouble();;
            return QVariant(price * quantity);
        }

        // Calculate quantity difference
        if(index.column() == fieldIndex("orig_qty_diff")) {
            int quantity = data(this->index(index.row(),fieldIndex("quantity"))).toInt();
            int originalQuantity = data(this->index(index.row(),fieldIndex("orig_qty"))).toInt();
            return QVariant(quantity - originalQuantity);
        }

        // Calculate price difference
        if(index.column() == fieldIndex("orig_price_diff")) {
            double price = data(this->index(index.row(),fieldIndex("unit_price"))).toDouble();
            double originalPrice = data(this->index(index.row(),fieldIndex("orig_price"))).toDouble();
            return QVariant(price - originalPrice);
        }

    }

    if (role == Qt::BackgroundRole) {

        // Calculate quantity difference
        if(index.column() == fieldIndex("orig_qty_diff")) {
            int difference = data(index, Qt::DisplayRole).toInt();
            if(difference > 0)
                return QColor(171, 248, 171);
            else if(difference < 0)
                return QColor(255, 178, 178);
        }

        // Calculate price difference
        if(index.column() == fieldIndex("orig_price_diff")) {
            double difference = data(index, Qt::DisplayRole).toDouble();
            if(difference > 0.0)
                return QColor(171, 248, 171);
            else if(difference < 0)
                return QColor(255, 178, 178);
        }
    }

    return QSqlTableModel::data(index, role);
}
