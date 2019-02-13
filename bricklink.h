#ifndef BRICKLINK_H
#define BRICKLINK_H

#include <QtNetworkAuth>

class DataModel : public QObject
{
    Q_OBJECT
public:
    enum Field {
        InventoryID = 0,
        ItemNo,
        ItemName,
        ItemType,
        CategoryID,
        CategoryName,
        ColorId,
        Quantity,
        NewOrUsed,
        Completeness,
        UnitPrice,
        UnitPriceFinal,
        DispUnitPrice,
        DispUnitPriceFinal,
        CurrencyCode,
        DispCurrencyCode,
        Remarks,
        Description,
        Weight,
        BatchNumber
    };
    Q_ENUM(Field)

    const QString HEADERS[21] = {"ID", "Item No", "Item Name", "Item Type", "Category ID", "Category Name", "Color ID", "Color Name",
                           "Quantity", "New Or Used", "Completeness", "UnitPrice", "UnitPriceFinal", "DispUnitPrice", "DispUnitPriceFinal",
                           "CurrencyCode", "DispCurrencyCode", "Remarks", "Description", "Weight", "BatchNumber"};

    static QString getHeader(Field);

private:
    QMap<Field, bool> initColumnNames() {
        QMap<Field, bool> map;
        map.insert(Field::InventoryID, false);
        map.insert(Field::ItemNo, true);
        map.insert(Field::ItemName, true);
        map.insert(Field::ItemType, true);
        map.insert(Field::CategoryID, false);
        map.insert(Field::CategoryName, true);
        map.insert(Field::ColorId, false);
        map.insert(Field::Quantity, true);
        map.insert(Field::NewOrUsed, true);
        map.insert(Field::Completeness, false);
        map.insert(Field::UnitPrice, true);
        map.insert(Field::UnitPriceFinal, false);
        map.insert(Field::DispUnitPrice, false);
        map.insert(Field::DispUnitPriceFinal, false);
        map.insert(Field::CurrencyCode, false);
        map.insert(Field::DispCurrencyCode, false);
        map.insert(Field::Remarks, true);
        map.insert(Field::Description, true);
        map.insert(Field::Weight, false);
        map.insert(Field::BatchNumber, false);
        return map;
    }
    QMap<Field, bool> columnVisibility = initColumnNames();
};


class BrickLink : public QOAuth1
{
    Q_OBJECT

public:
    BrickLink(QObject *parent = nullptr);
    bool checkConnection(QObject *parent = nullptr);
    void importCategories();
    void importColors();
    void importOrderItem(int orderID);
    void importOrders(bool filed);
    void importUserInventory();
    static QJsonArray validateBricklinkResponse(QObject* obj);

signals:
    void dataBaseUpdated();

private slots:
    void parseJsonOrderItem(int orderID);

private:
    Q_DISABLE_COPY(BrickLink)
    void parseJsonCategories();
    void parseJsonColors();
    void parseJsonOrders();
//    void parseJsonOrderItem();
    void parseJsonUserInventory();
};

#endif // BRICKLINK_H
