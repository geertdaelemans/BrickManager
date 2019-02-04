#include "inventorytablemodel.h"
#include "mainwindow.h"
#include "inventory.h"
#include "sqldatabase.h"

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
    return getDataForTable(index);
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

QString InventoryTableModel::getDataForTable(const QModelIndex &index) const
{
    auto item = parts.begin();
    std::advance(item, index.row());
    switch (index.column())
    {
    case Inventory::InventoryID:
        return QString::number(item->inventory_id);
    case Inventory::ItemNo:
        return item->item.no;
    case Inventory::ItemName:
        return item->item.name;
    case Inventory::ItemType:
        return item->item.type;
    case Inventory::CategoryID:
        return QString::number(item->item.category_id);
    case Inventory::CategoryName:
        return item->item.category_name;
    case Inventory::ColorId:
        return QString::number(item->color_id);
    case Inventory::ColorName:
        return item->color_name;
    case Inventory::Quantity:
        return QString::number(item->quantity);
    case Inventory::NewOrUsed:
        return item->new_or_used;
    case Inventory::Completeness:
        return item->completeness;
    case Inventory::UnitPrice:
        return QString::number(item->unit_price);
    case Inventory::UnitPriceFinal:
        return QString::number(item->unit_price_final);
    case Inventory::DispUnitPrice:
        return QString::number(item->disp_unit_price);
    case Inventory::DispUnitPriceFinal:
        return QString::number(item->disp_unit_price_final);
    case Inventory::CurrencyCode:
        return item->currency_code;
    case Inventory::DispCurrencyCode:
        return item->disp_currency_code;
    case Inventory::Remarks:
        return item->remarks;
    case Inventory::Description:
        return item->description;
    case Inventory::Weight:
        return QString::number(item->weight);
    case Inventory::BatchNumber:
        return QString::number(item->batchNumber);
    default:
        return QString();
    }
}

void InventoryTableModel::updateParts(int orderID)
{
    QUrl url;
    if(orderID == 0) {
        url = "https://api.bricklink.com/api/store/v1/inventories";

    } else {
        url = "https://api.bricklink.com/api/store/v1/orders/" + QString::number(orderID) + "/items";
    }
    QVariantMap parameters;
    QNetworkReply *reply = bricklink.get(url, parameters);
    if(orderID == 0) {
        connect(reply, &QNetworkReply::finished, this, &InventoryTableModel::parseJsonUserInventory);
    } else {
        connect(reply, &QNetworkReply::finished, this, &InventoryTableModel::parseJsonOrderItem);
    }
}


void InventoryTableModel::parseJsonOrderItem()
{
    QJsonArray batchArray = BrickLink::validateBricklinkResponse(sender());
    int batchNumber = 0;
    foreach(const QJsonValue &batch, batchArray) {
        batchNumber++;
        QJsonArray itemArray = batch.toArray();
        beginInsertRows(QModelIndex(), 0, itemArray.size() - 1);
        auto before = parts.end();
        foreach(const QJsonValue &item, itemArray) {
            QJsonObject object = item.toObject();
            before = parts.insert(before, OrderItem{
                object.value("inventory_id").toInt(),
                Item{
                    object.value("item").toObject().value("no").toString(),
                    object.value("item").toObject().value("name").toString(),
                    object.value("item").toObject().value("type").toString(),
                    object.value("item").toObject().value("category_id").toInt(),
                    SqlDatabase::getCategoryById(object.value("item").toObject().value("category_id").toInt())
                },
                object.value("color_id").toInt(),
                SqlDatabase::getColorById(object.value("color_id").toInt()),
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

void InventoryTableModel::parseJsonUserInventory()
{
    QJsonArray itemArray = BrickLink::validateBricklinkResponse(sender());
    beginInsertRows(QModelIndex(), 0, itemArray.size() - 1);
    auto before = parts.end();
    foreach(const QJsonValue &item, itemArray) {
        QJsonObject object = item.toObject();
        before = parts.insert(before, OrderItem{
            object.value("inventory_id").toInt(),
            Item{
                object.value("item").toObject().value("no").toString(),
                object.value("item").toObject().value("name").toString(),
                object.value("item").toObject().value("type").toString(),
                object.value("item").toObject().value("category_id").toInt(),
                SqlDatabase::getCategoryById(object.value("item").toObject().value("category_id").toInt())
            },
            object.value("color_id").toInt(),
            SqlDatabase::getColorById(object.value("color_id").toInt()),
            object.value("quantity").toInt(),
            object.value("new_or_used").toString(),
            "N/A",
            object.value("unit_price").toVariant().toDouble(),
            0.00,
            0.00,
            0.00,
            "N/A",
            "N/A",
            object.value("remarks").toString(),
            object.value("description").toString(),
            object.value("my_weight").toVariant().toDouble(),
            0
        });
        std::advance(before, 1);
    }
    endInsertRows();
    return;
}
