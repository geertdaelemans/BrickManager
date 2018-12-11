#ifndef ORDERSTABLEMODEL_H
#define ORDERSTABLEMODEL_H

#include "bricklink.h"

#include <QtCore>

class OrdersTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OrdersTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void clearOrders();
    QAbstractOAuth::Status status() const;

    bool filed = false;

public slots:
    void updateOrders();
    void switchFiled(int arg1);

private:
    Q_DISABLE_COPY(OrdersTableModel)

    void parseJson();

    struct Order {
        int order_id;
        QDateTime date_ordered;
        QString seller_name;
        QString store_name;
        QString buyer_name;
        int total_count;
        int unique_count;
        QString status;
        QString payment_method;
        QString payment_status;
        QDateTime payment_date_paid;
        QString payment_currency_code;
        double cost_subtotal;
        double cost_grand_total;
        QString cost_currency_code;
    };

    QList<Order> orders;
    BrickLink bricklink;
};

#endif // ORDERSTABLEMODEL_H
