#ifndef INVENTORY_H
#define INVENTORY_H

#include <QWidget>
#include "inventorytablemodel.h"

namespace Ui {
class Inventory;
}

class Inventory : public QWidget
{
    Q_OBJECT

public:
    explicit Inventory(QWidget *parent = nullptr, int orderID = 0);
    ~Inventory();

private:
    InventoryTableModel *model;
    Ui::Inventory *ui;
};

#endif // INVENTORY_H
