#include "ordersdialog.h"
#include "orderstablemodel.h"
#include "ui_ordersdialog.h"

OrdersDialog::OrdersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrdersDialog)
{
    model = new OrdersTableModel(this);
    ui->setupUi(this);
    ui->view->setModel(model);
    ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->view->setAlternatingRowColors(true);
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

void OrdersDialog::on_view_clicked(const QModelIndex &index)
{
    qDebug() << "Clicked on order#" + ui->view->model()->data(ui->view->model()->index(index.row(), 0)).toString();
}
