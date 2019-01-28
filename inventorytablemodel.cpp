#include "inventorytablemodel.h"
#include "mainwindow.h"
#include "category.h"

#include <QtGui>
#include <QtCore>
#include <QtWidgets>

InventoryTableModel::InventoryTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int InventoryTableModel::rowCount(const QModelIndex &parent) const
{
#if defined(QT_DEBUG)
    Q_ASSERT(!parent.isValid());
#else
    Q_UNUSED(parent)
#endif
    return parts.size();
}

QVariant InventoryTableModel::data(const QModelIndex &index, int role) const
{

    if (role != Qt::DisplayRole)
        return QVariant();
    auto it = parts.begin();
    std::advance(it, index.row());
    switch (index.column())
    {
    case 0:
        return QString::number(it->inventory_id);
    case 1:
        return it->item_no;
    case 2:
        return it->item_name;
    case 3:
        return it->item_type;
    case 4:
        return it->category_name;
    case 5:
        return it->color_name;
    case 6:
        return QString::number(it->quantity);
    case 7:
        return it->new_or_used;
    case 8:
        return it->completeness;
    case 9:
        return QString::number(it->unit_price);
    case 10:
        return QString::number(it->unit_price_final);
    case 11:
        return QString::number(it->disp_unit_price);
    case 12:
        return QString::number(it->disp_unit_price_final);
    case 13:
        return it->currency_code;
    case 14:
        return it->disp_currency_code;
    case 15:
        return it->remarks;
    case 16:
        return it->description;
    case 17:
        return QString::number(it->weight);
    case 18:
        return QString::number(it->batchNumber);
    }
    return QVariant();
}

int InventoryTableModel::columnCount(const QModelIndex &) const
{
    return 19;
}

QVariant InventoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QStringLiteral("ID");
        case 1:
            return QStringLiteral("Item Number");
        case 2:
            return QStringLiteral("Item Name");
        case 3:
            return QStringLiteral("Item Type");
        case 4:
            return QStringLiteral("Category");
        case 5:
            return QStringLiteral("Color");
        case 6:
            return QStringLiteral("Quantity");
        case 7:
            return QStringLiteral("Condition");
        case 8:
            return QStringLiteral("Completeness");
        case 9:
            return QStringLiteral("Unit Price");
        case 10:
            return QStringLiteral("Unit Price Final");
        case 11:
            return QStringLiteral("Unit Price Buyer");
        case 12:
            return QStringLiteral("Unit Price Buyer Final");
        case 13:
            return QStringLiteral("Currency");
        case 14:
            return QStringLiteral("Currency User");
        case 15:
            return QStringLiteral("Remarks");
        case 16:
            return QStringLiteral("Description");
        case 17:
            return QStringLiteral("Weight");
        case 18:
            return QStringLiteral("Batch");
        }
    }
    return section;
}

void InventoryTableModel::clearParts()
{
   this->beginResetModel();
   parts.clear();
   this->endResetModel();
}

void InventoryTableModel::updateParts(int orderID)
{
    QUrl url("https://api.bricklink.com/api/store/v1/orders/" + QString::number(orderID) + "/items");
    QVariantMap parameters;
    QNetworkReply *reply = bricklink.get(url, parameters);
    connect(reply, &QNetworkReply::finished, this, &InventoryTableModel::parseJson);
}

void InventoryTableModel::parseJson()
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
        QJsonArray batchArray = jsonObject.value("data").toArray();
        int batchNumber = 0;
        foreach(const QJsonValue &batch, batchArray) {
            batchNumber++;
            QJsonArray itemArray = batch.toArray();
            beginInsertRows(QModelIndex(), 0, itemArray.size() - 1);
            auto before = parts.end();
            foreach(const QJsonValue &item, itemArray) {
                QJsonObject object = item.toObject();
                before = parts.insert(before, Part{
                    object.value("inventory_id").toInt(),
                    object.value("item").toObject().value("no").toString(),
                    object.value("item").toObject().value("name").toString(),
                    object.value("item").toObject().value("type").toString(),
                    object.value("item").toObject().value("category_id").toInt(),
                    cat.getCategory(object.value("item").toObject().value("category_id").toInt()),
                    object.value("color_id").toInt(),
                    col.getColor(object.value("color_id").toInt()),
                    object.value("quantity").toInt(),
                    object.value("new_or_used").toString(),
                    object.value("completeness").toString(),
                    object.value("unit_price").toVariant().toDouble(),
                    object.value("unit_price_final").toVariant().toDouble(),
                    object.value("disp_unit_price").toVariant().toDouble(),
                    object.value("disp_unit_price_final").toVariant().toDouble(),
                    object.value("currency_code").toString(),
                    object.value("disp_currency_code").toString(),
                    object.value("remarks").toString(),
                    object.value("description").toString(),
                    object.value("weight").toVariant().toDouble(),
                    batchNumber
                });
                std::advance(before, 1);
            }
            endInsertRows();
        }
        return;
    }
}