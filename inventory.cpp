#include "inventory.h"
#include "ui_inventory.h"

Inventory::Inventory(QWidget *parent, int orderID) :
    QWidget(parent),
    ui(new Ui::Inventory)
{
    model = new InventoryTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    ui->setupUi(this);
    ui->tableView->setModel(proxyModel);

    model->updateParts(orderID);
}

Inventory::~Inventory()
{
    delete ui;
}
