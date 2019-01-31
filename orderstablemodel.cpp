#include "orderstablemodel.h"
#include "ordersdialog.h"

#include <QtGui>
#include <QtCore>
#include <QtWidgets>

OrdersTableModel::OrdersTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    p_parent = parent;
}

int OrdersTableModel::rowCount(const QModelIndex &parent) const
{
#if defined(QT_DEBUG)
    Q_ASSERT(!parent.isValid());
#else
    Q_UNUSED(parent)
#endif
    return orders.size();
}

QVariant OrdersTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    auto it = orders.begin();
    std::advance(it, index.row());
    switch (index.column())
    {
    case OrdersDialog::OrderID:
        return QString::number(it->order_id);
    case OrdersDialog::DateOrdered:
        return it->date_ordered;
    case OrdersDialog::SellerName:
        return it->seller_name;
    case OrdersDialog::StoreName:
        return it->store_name;
    case OrdersDialog::BuyerName:
        return it->buyer_name;
    case OrdersDialog::TotalCount:
        return it->total_count;
    case OrdersDialog::UniqueCount:
        return it->unique_count;
    case OrdersDialog::Status:
        return it->status;
    case OrdersDialog::PaymentMethod:
        return it->payment_method;
    case OrdersDialog::PaymentStatus:
        return it->payment_status;
    case OrdersDialog::PaymentDatePaid:
        return it->payment_date_paid;
    case OrdersDialog::PaymentCurrencyCode:
        return it->payment_currency_code;
    case OrdersDialog::CostSubtotal:
        return it->cost_subtotal;
    case OrdersDialog::CostGrandTotal:
        return it->cost_grand_total;
    case OrdersDialog::CostCurrencyCode:
        return it->cost_currency_code;
    }
    return QVariant();
}

int OrdersTableModel::columnCount(const QModelIndex &) const
{
    return 15;
}

QVariant OrdersTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        OrdersDialog::Field field = static_cast<OrdersDialog::Field>(section);
        qobject_cast<OrdersDialog *> (p_parent)->setVisibility(field);
        return OrdersDialog::getHeader(field);
    }
    return section;
}

QAbstractOAuth::Status OrdersTableModel::status() const
{
    return bricklink.status();
}

void OrdersTableModel::clearOrders()
{
   this->beginResetModel();
   orders.clear();
   this->endResetModel();
}

void OrdersTableModel::switchFiled(int state)
{
    filed = (state == Qt::Checked);
}

void OrdersTableModel::updateOrders()
{
    QUrl url("https://api.bricklink.com/api/store/v1/orders");
    QVariantMap parameters;
    parameters.insert("direction", "in");
    parameters.insert("filed", (filed ? "true" : "false"));
    QNetworkReply *reply = bricklink.get(url, parameters);

    connect(reply, &QNetworkReply::finished, this, &OrdersTableModel::parseJson);
}

void OrdersTableModel::parseJson()
{
    QJsonParseError parseError;
    auto reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    const auto data = reply->readAll();
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error) {
        qCritical() << "BrickLinkViewModel::parseJson. Error at:" << parseError.offset
                    << parseError.errorString();
        return;
    } else {
        QString strReply = static_cast<QString>(data);
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        const auto array = jsonObject["data"].toArray();
        if (array.size()) {
            clearOrders();
            beginInsertRows(QModelIndex(), 0, array.size() - 1);
            auto before = orders.begin();
            for (const auto &value : array) {
                Q_ASSERT(value.isObject());
                const auto object = value.toObject();
                before = orders.insert(before, Order{
                    object.value("order_id").toInt(),
                    QDateTime::fromString(object.value("date_ordered").toString(), "yyyy-MM-dd'T'HH:mm:ss.zzz'Z'"),
                    object.value("seller_name").toString(),
                    object.value("store_name").toString(),
                    object.value("buyer_name").toString(),
                    object.value("total_count").toInt(),
                    object.value("unique_count").toInt(),
                    object.value("status").toString(),
                    object.value("payment").toObject().value("method").toString(),
                    object.value("payment").toObject().value("status").toString(),
                    QDateTime::fromString(object.value("payment").toObject().value("date_paid").toString(), "yyyy-MM-dd'T'HH:mm:ss.zzz'Z'"),
                    object.value("payment").toObject().value("currency_code").toString(),
                    object.value("cost").toObject().value("subtotal").toVariant().toDouble(),
                    object.value("cost").toObject().value("grand_total").toVariant().toDouble(),
                    object.value("cost").toObject().value("currency_code").toString()
                });
                std::advance(before, 1);
            }
            endInsertRows();
        }
        return;
    }
}
