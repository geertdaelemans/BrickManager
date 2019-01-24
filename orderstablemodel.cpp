#include "orderstablemodel.h"

#include <QtGui>
#include <QtCore>
#include <QtWidgets>

OrdersTableModel::OrdersTableModel(QObject *parent) : QAbstractTableModel(parent)
{
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
    case 0:
        return QString::number(it->order_id);
    case 1:
        return it->date_ordered;
    case 2:
        return it->seller_name;
    case 3:
        return it->store_name;
    case 4:
        return it->buyer_name;
    case 5:
        return it->total_count;
    case 6:
        return it->unique_count;
    case 7:
        return it->status;
    case 8:
        return it->payment_method;
    case 9:
        return it->payment_status;
    case 10:
        return it->payment_date_paid;
    case 11:
        return it->payment_currency_code;
    case 12:
        return it->cost_subtotal;
    case 13:
        return it->cost_grand_total;
    case 14:
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
        switch (section) {
        case 0:
            return QStringLiteral("Order Id");
        case 1:
            return QStringLiteral("Date Ordered");
        case 2:
            return QStringLiteral("Seller Name");
        case 3:
            return QStringLiteral("Store Name");
        case 4:
            return QStringLiteral("Buyer Name");
        case 5:
            return QStringLiteral("Total Count");
        case 6:
            return QStringLiteral("Unique Count");
        case 7:
            return QStringLiteral("Status");
        case 8:
            return QStringLiteral("Method");
        case 9:
            return QStringLiteral("Status");
        case 10:
            return QStringLiteral("Date Paid");
        case 11:
            return QStringLiteral("Currency");
        case 12:
            return QStringLiteral("Subtotal");
        case 13:
            return QStringLiteral("Grand Total");
        case 14:
            return QStringLiteral("Currency");
        }
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
