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

    enum Field {
        OrderID = 0,
        DateOrdered,
        SellerName,
        StoreName,
        BuyerName,
        TotalCount,
        UniqueCount,
        Status,
        PaymentMethod,
        PaymentStatus,
        PaymentDatePaid,
        PaymentCurrencyCode,
        CostSubtotal,
        CostGrandTotal,
        CostCurrencyCode
    };
    Q_ENUM(Field)

    static QString getHeader(Field);
    bool getVisibilityColumn(Field);
    void setVisibility(Field);
    void setVisibility(Field, bool);
    QWidget *parent;
    void openTab();

signals:
    void ordersSelected(QList<QString> orderIDs);

private slots:
    void slotCustomMenuRequested(QPoint);
    void setVisibilityFromCheckBox();
    void on_checkBoxFiled_stateChanged(int arg1);
    void on_pushButton_clicked();
    void on_view_doubleClicked(const QModelIndex &index);

private:
    QMap<Field, bool> initColumnNames() {
        QMap<Field, bool> map;
        map.insert(Field::OrderID, true);
        map.insert(Field::DateOrdered, true);
        map.insert(Field::SellerName, false);
        map.insert(Field::StoreName, false);
        map.insert(Field::BuyerName, true);
        map.insert(Field::TotalCount, true);
        map.insert(Field::UniqueCount, true);
        map.insert(Field::Status, true);
        map.insert(Field::PaymentMethod, false);
        map.insert(Field::PaymentStatus, false);
        map.insert(Field::PaymentDatePaid, false);
        map.insert(Field::PaymentCurrencyCode, true);
        map.insert(Field::CostSubtotal, true);
        map.insert(Field::CostGrandTotal, true);
        map.insert(Field::CostCurrencyCode, false);
        return map;
    }
    QMap<Field, bool> columnVisibility = initColumnNames();
    OrdersTableModel *model;
    QSortFilterProxyModel *proxyModel;
    Ui::OrdersDialog *ui;
};

#endif // ORDERSDIALOG_H
