#ifndef INVENTORYTABLEMODEL_H
#define INVENTORYTABLEMODEL_H

#include "bricklink.h"
#include "category.h"
#include "colors.h"

#include <QtCore>

class MainWindow;

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
    Category cat;
    Colors col;

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
        QString category_name;
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
        int batchNumber;
    };

    QList<Part> parts;
    BrickLink bricklink;

};

#endif // INVENTORYTABLEMODEL_H
