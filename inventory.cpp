#include "inventory.h"
#include "inventorytablemodel.h"
#include "ui_inventory.h"

Inventory::Inventory(QWidget *parent, int orderID) :
    QWidget(parent),
    ui(new Ui::Inventory)
{
    model = new InventoryTableModel(this);

    ui->setupUi(this);
    ui->tableView->setModel(model);
    model->updateParts(orderID);
}

Inventory::~Inventory()
{
    delete ui;
}
