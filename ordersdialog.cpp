#include "ordersdialog.h"
#include "orderstablemodel.h"
#include "ui_bricklinkorders.h"

OrdersDialog::OrdersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrdersDialog)
{
    model = new OrdersTableModel(this);
    ui->setupUi(this);
    ui->view->setModel(model);
}

OrdersDialog::~OrdersDialog()
{
    delete ui;
}

void OrdersDialog::on_pushButton_clicked()
{
    model->updateOrders();
}

void OrdersDialog::on_checkBoxFiled_stateChanged(int arg1)
{
    model->switchFiled(arg1);
}

void OrdersDialog::updateStatus() {
    qDebug() << "clicked";
}
