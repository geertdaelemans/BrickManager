#ifndef ORDERSDIALOG_H
#define ORDERSDIALOG_H

#include <QDialog>
#include "orderstablemodel.h"

namespace Ui {
class OrdersDialog;
}

class OrdersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrdersDialog(QWidget *parent = nullptr);
    ~OrdersDialog();

    void updateStatus();
private slots:
    void on_pushButton_clicked();
    void on_checkBoxFiled_stateChanged(int arg1);

private:
    OrdersTableModel *model;
    Ui::OrdersDialog *ui;
};

#endif // ORDERSDIALOG_H
