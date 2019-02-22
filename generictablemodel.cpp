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
            return QVariant(qvariant_cast<QDateTime>(QSqlTableModel::data(index, role)).toString("yyyy-MM-dd hh:mm:ss"));

        // Calculate total
        if(index.column() == fieldIndex("total")) {
            int quantity = data(this->index(index.row(),fieldIndex("quantity"))).toInt();
            double price = data(this->index(index.row(),fieldIndex("unit_price"))).toDouble();;
            return QVariant(price * quantity);
        }

        // Calculate quantity difference
        if(index.column() == fieldIndex("OrigQtyDiff")) {
            int quantity = data(this->index(index.row(),fieldIndex("quantity"))).toInt();
            int originalQuantity = data(this->index(index.row(),fieldIndex("OrigQty"))).toInt();
            return QVariant(quantity - originalQuantity);
        }

        // Calculate price difference
        if(index.column() == fieldIndex("OrigPriceDiff")) {
            double price = data(this->index(index.row(),fieldIndex("unit_price"))).toDouble();
            double originalPrice = data(this->index(index.row(),fieldIndex("OrigPrice"))).toDouble();
            return QVariant(price - originalPrice);
        }

//        // Insert checkboxes for boolean values
//        if((index.column() == fieldIndex("is_retain")) || (index.column() == fieldIndex("is_stock_room"))) {
//            return QVariant("");
//        }

    }

    if (role == Qt::BackgroundRole) {

        // Calculate quantity difference
        if(index.column() == fieldIndex("OrigQtyDiff")) {
            int difference = data(index, Qt::DisplayRole).toInt();
            if(difference > 0)
                return QColor(171, 248, 171);
            else if(difference < 0)
                return QColor(255, 178, 178);
        }

        // Calculate price difference
        if(index.column() == fieldIndex("OrigPriceDiff")) {
            double difference = data(index, Qt::DisplayRole).toDouble();
            if(difference > 0.0)
                return QColor(171, 248, 171);
            else if(difference < 0)
                return QColor(255, 178, 178);
        }
    }

    if (role == Qt::CheckStateRole) {

        // Insert checkboxes for boolean values
        if((index.column() == fieldIndex("is_retain")) || (index.column() == fieldIndex("is_stock_room"))) {
            bool aBool = data(index, Qt::DisplayRole).toBool();
            if (aBool)
                    return Qt::Checked;
            else
                    return Qt::Unchecked;
        }

    }

    return QSqlTableModel::data(index, role);
}
