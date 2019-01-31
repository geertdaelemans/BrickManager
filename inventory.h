#ifndef INVENTORY_H
#define INVENTORY_H

#include <QWidget>
#include "inventorytablemodel.h"

class MainWindow;

namespace Ui {
class Inventory;
}

class Inventory : public QWidget
{
    Q_OBJECT

public:
    explicit Inventory(QWidget *parent = nullptr, int orderID = 0);
    ~Inventory();

    enum Field {
        InventoryID = 0,
        ItemNo,
        ItemName,
        ItemType,
        CategoryID,
        CategoryName,
        ColorId,
        ColorName,
        Quantity,
        NewOrUsed,
        Completeness,
        UnitPrice,
        UnitPriceFinal,
        DispUnitPrice,
        DispUnitPriceFinal,
        CurrencyCode,
        DispCurrencyCode,
        Remarks,
        Description,
        Weight,
        BatchNumber
    };
    Q_ENUM(Field)

    static QString getHeader(Field);
    bool getVisibilityColumn(Field);
    void setVisibility(Field);
    void setVisibility(Field, bool);

private slots:
    void slotCustomMenuRequested(QPoint);
    void setVisibilityFromCheckBox();

private:
    QMap<Field, bool> initColumnNames() {
        QMap<Field, bool> map;
        map.insert(Field::InventoryID, false);
        map.insert(Field::ItemNo, true);
        map.insert(Field::ItemName, true);
        map.insert(Field::ItemType, true);
        map.insert(Field::CategoryID, false);
        map.insert(Field::CategoryName, true);
        map.insert(Field::ColorId, false);
        map.insert(Field::ColorName, true);
        map.insert(Field::Quantity, true);
        map.insert(Field::NewOrUsed, true);
        map.insert(Field::Completeness, false);
        map.insert(Field::UnitPrice, true);
        map.insert(Field::UnitPriceFinal, false);
        map.insert(Field::DispUnitPrice, false);
        map.insert(Field::DispUnitPriceFinal, false);
        map.insert(Field::CurrencyCode, false);
        map.insert(Field::DispCurrencyCode, false);
        map.insert(Field::Remarks, true);
        map.insert(Field::Description, true);
        map.insert(Field::Weight, false);
        map.insert(Field::BatchNumber, false);
        return map;
    }
    QMap<Field, bool> columnVisibility = initColumnNames();
    InventoryTableModel *model;
    Ui::Inventory *ui;
    QSortFilterProxyModel *proxyModel;
};

#endif // INVENTORY_H
