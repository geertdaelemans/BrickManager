#include "inventorytablemodel.h"
#include "mainwindow.h"
#include "category.h"
#include "inventory.h"

#include <QtGui>
#include <QtCore>
#include <QtWidgets>

InventoryTableModel::InventoryTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    p_parent = parent;
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
    case Inventory::InventoryID:
        return QString::number(it->inventory_id);
    case Inventory::ItemNo:
        return it->item_no;
    case Inventory::ItemName:
        return it->item_name;
    case Inventory::ItemType:
        return it->item_type;
    case Inventory::CategoryID:
        return QString::number(it->category_id);
    case Inventory::CategoryName:
        return it->category_name;
    case Inventory::ColorId:
        return QString::number(it->color_id);
    case Inventory::ColorName:
        return it->color_name;
    case Inventory::Quantity:
        return QString::number(it->quantity);
    case Inventory::NewOrUsed:
        return it->new_or_used;
    case Inventory::Completeness:
        return it->completeness;
    case Inventory::UnitPrice:
        return QString::number(it->unit_price);
    case Inventory::UnitPriceFinal:
        return QString::number(it->unit_price_final);
    case Inventory::DispUnitPrice:
        return QString::number(it->disp_unit_price);
    case Inventory::DispUnitPriceFinal:
        return QString::number(it->disp_unit_price_final);
    case Inventory::CurrencyCode:
        return it->currency_code;
    case Inventory::DispCurrencyCode:
        return it->disp_currency_code;
    case Inventory::Remarks:
        return it->remarks;
    case Inventory::Description:
        return it->description;
    case Inventory::Weight:
        return QString::number(it->weight);
    case Inventory::BatchNumber:
        return QString::number(it->batchNumber);
    }
    return QVariant();
}

int InventoryTableModel::columnCount(const QModelIndex &) const
{
    return 21;
}

QVariant InventoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();    

    if (orientation == Qt::Horizontal) {
        Inventory::Field field = static_cast<Inventory::Field>(section);
        qobject_cast<Inventory *> (p_parent)->setVisibility(field);
        return Inventory::getHeader(field);
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
