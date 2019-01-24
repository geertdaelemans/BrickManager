#include "ordersdialog.h"
#include "orderstablemodel.h"
#include "ui_ordersdialog.h"

OrdersDialog::OrdersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrdersDialog)
{
    model = new OrdersTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->sort(1, Qt::DescendingOrder);

    ui->setupUi(this);
    ui->view->setModel(proxyModel);

    model->updateOrders();
}

OrdersDialog::~OrdersDialog()
{
    delete ui;
}

void OrdersDialog::on_checkBoxFiled_stateChanged(int arg1)
{
    model->switchFiled(arg1);
    model->updateOrders();
}

void OrdersDialog::on_pushButton_clicked()
{
    QItemSelectionModel *select = ui->view->selectionModel();
    QModelIndexList rows = select->selectedRows(); // return selected row(s)
    QList<QString> list;
    foreach(QModelIndex index, rows) {
        list.append(ui->view->model()->data(ui->view->model()->index(index.row(), 0)).toString());
    }
    emit ordersSelected(list);
}
