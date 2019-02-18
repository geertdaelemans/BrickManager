#ifndef ORDERSDIALOG_H
#define ORDERSDIALOG_H

#include <QDialog>
#include "datamodels.h"
#include "bricklink.h"

namespace Ui {
class OrdersDialog;
}

class OrdersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrdersDialog(QWidget *parent = nullptr);
    ~OrdersDialog() override;
    void openTab();

signals:
    void ordersSelected(QList<QString> orderIDs);

private slots:
    void slotCustomMenuRequested(QPoint);
    void on_checkBoxFiled_stateChanged(int arg1);
    void on_pushButton_clicked();
    void on_view_doubleClicked(const QModelIndex &index);

private:
    void reject() override;
    void setVisibilityFromCheckBox();
    void showError(const QSqlError &err);
    TableModel *p_tableModel;
    QSqlRelationalTableModel *model;
    QSortFilterProxyModel *proxyModel;
    Ui::OrdersDialog *ui;
    BrickLink bricklink;
};

#endif // ORDERSDIALOG_H
