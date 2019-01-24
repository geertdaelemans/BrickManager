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
    QWidget *parent;
    void openTab();

signals:
    void ordersSelected(QList<QString> orderIDs);

private slots:
    void on_checkBoxFiled_stateChanged(int arg1);

    void on_pushButton_clicked();

private:
    OrdersTableModel *model;
    QSortFilterProxyModel *proxyModel;
    Ui::OrdersDialog *ui;
};

#endif // ORDERSDIALOG_H
