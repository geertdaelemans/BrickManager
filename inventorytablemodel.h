#ifndef INVENTORYTABLEMODEL_H
#define INVENTORYTABLEMODEL_H

#include "bricklink.h"

#include <QtCore>

class InventoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    InventoryTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void clearParts();

public slots:
    void updateParts(int orderID);

private:
    Q_DISABLE_COPY(InventoryTableModel)

    void parseJson();

    struct Part {
        int inventory_id;
        QString item_no;
        QString item_name;
        QString item_type;
        int category_id;
        int color_id;
        QString color_name;
        int quantity;
        QString new_or_used;
        QString completeness;
        double unit_price;
        double unit_price_final;
        double disp_unit_price;
        double disp_unit_price_final;
        QString currency_code;
        QString disp_currency_code;
        QString remarks;
        QString description;
        double weight;
    };

    QList<Part> parts;
    BrickLink bricklink;
};

#endif // INVENTORYTABLEMODEL_H
