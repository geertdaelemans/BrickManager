#include "datamodel.h"
#include "bricklink.h"


DataModel::DataModel(Tables table, QString tableName)
{
    QString prefix = ItemType::getTablePrefix();
    switch (table)
    {
    case Tables::indextable:
        sqlTable = "indextable";
        columns[0] = Column("id", "id", tr("ID"), "integer", false, 50);
        columns[1] = Column("sqltable", "sqltable", tr("SQL Table"), "varchar", false, 173);
        columns[2] = Column("name", "name", tr("Name"), "varchar", false, 173);
        sortColumn = 0;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::brickstock:
        sqlTable = tableName;
        columns[0] = Column("id", "id", tr("ID"), "integer", false, 100);
        columns[1] = Column("Status", "Status", tr("Status"), "varchar", true, 21);
        columns[2] = Column("Image", "Image", tr("Image"), "varchar", true, 45);
        columns[3] = Column("item_no", "ItemID", tr("Part #"), "varchar", true, 65);
        columns[4] = Column("item_name", "ItemName", tr("Description"), "varchar", true, 173);
        columns[5] = Column("Condition", "Condition", tr("Cond."), "varchar", true, 35);
        columns[6] = Column("color_name", "ColorName", tr("Color"), "integer", true, 95);
        columns[7] = Column("quantity", "Qty", tr("Qty."), "integer", true, 35);
        columns[8] = Column("unit_price", "Price", tr("Price"), "double", true, 53);
        columns[9] = Column("total", "Total", tr("Total"), "double", true, 53);
        columns[10] = Column("Bulk", "Bulk", tr("Bulk"), "integer", true, 35);
        columns[11] = Column("Sale", "Sale", tr("Sale"), "integer", true, 35);
        columns[12] = Column("Comments", "Comments", tr("Comments"), "varchar", true, 53);
        columns[13] = Column("Remarks", "Remarks", tr("Remarks"), "varchar", true, 53);
        columns[14] = Column("category_name", "CategoryName", tr("Category"), "integer", true, 77);
        columns[15] = Column("item_type", "ItemTypeName", tr("Item Type"), "varchar", true, 77);
        columns[16] = Column("TQ1", "TQ1", tr("Tier Q1"), "integer", true, 35);
        columns[17] = Column("TP1", "TP1", tr("Tier P1"), "double", true, 53);
        columns[18] = Column("TQ2", "TQ2", tr("Tier Q2"), "integer", true, 35);
        columns[19] = Column("TP2", "TP2", tr("Tier P2"), "double", true, 53);
        columns[20] = Column("TQ3", "TQ3", tr("Tier Q3"), "integer", true, 35);
        columns[21] = Column("TP3", "TP3", tr("Tier P3"), "double", true, 53);
        columns[22] = Column("LotID", "LotID", tr("Lot ID"), "integer", true, 21);
        columns[23] = Column("is_retain", "Retain", tr("Retain"), "bool", true, 21);
        columns[24] = Column("is_stock_room", "Stockroom", tr("Stockroom"), "bool", true, 21);
        columns[25] = Column("Reserved", "Reserved", tr("Reserved"), "varchar", true, 21);
        columns[26] = Column("TotalWeight", "TotalWeight", tr("Weight"), "double", true, 21);
        columns[27] = Column("Year", "Year", tr("Year"), "integer", true, 21);
        columns[28] = Column("orig_qty", "OrigQty", tr("Qty. Orig"), "integer", true, 35);
        columns[29] = Column("orig_qty_diff", "OrigQtyDiff", tr("Qty. Diff"), "integer", true, 35);
        columns[30] = Column("orig_price", "OrigPrice", tr("Pr. Orig"), "double", true, 53);
        columns[31] = Column("orig_price_diff", "OrigPriceDiff", tr("Pr. Diff"), "double", true, 53);
        columns[32] = Column("category_id", "CategoryID", tr("Category ID"), "varchar", false, 77);    // Extra's should be ignored
        columns[33] = Column("color_id", "ColorID", tr("Color ID"), "varchar", false, 95);             // Extra's should be ignored
        columns[34] = Column("ItemTypeID", "ItemTypeID", tr("Item Type ID"), "varchar", false, 35);         // Extra's should be ignored
        sortColumn = 2;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::categories:
        sqlTable = "categories";
        columns[0] = Column("id", "CATEGORY", tr("ID"), "integer", false, 50);
        columns[1] = Column("category_name", "CATEGORYNAME", tr("Name"), "varchar", true, 300);
        columns[2] = Column(prefix + "book", "books", tr("Books"), "bool", false, 50);
        columns[3] = Column(prefix + "original_box", "boxes", tr("Original Boxes"), "bool", false, 50);
        columns[4] = Column(prefix + "catalog", "catalogs", tr("Catalogs"), "bool", false, 50);
        columns[5] = Column(prefix + "gear", "gear", tr("Gear"), "bool", false, 50);
        columns[6] = Column(prefix + "instruction", "instructions", tr("Instructions"), "bool", false, 50);
        columns[7] = Column(prefix + "minifig", "minifigs", tr("Minifigs"), "bool", false, 50);
        columns[8] = Column(prefix + "part", "parts", tr("Parts"), "bool", false, 50);
        columns[9] = Column(prefix + "set", "sets", tr("Sets"), "bool", false, 50);
        sortColumn = 1;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::colors:
        sqlTable = "colors";
        columns[0] = Column("id", "COLOR", tr("ID"), "integer", false, 50);
        columns[1] = Column("color_name", "COLORNAME", tr("Name"), "varchar", true, 200);
        columns[2] = Column("color_code", "COLORRGB", tr("Code"), "varchar", false, 80);
        columns[3] = Column("color_type", "COLORTYPE", tr("Type"), "varchar", true, 100);
        sortColumn = 1;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::orderitem:
        sqlTable = "orderitem" + tableName;
        columns[0] = Column("inventory_id", "inventory_id", tr("ID"), "integer", false, 80);                            // The ID of the inventory that includes the item
        columns[1] = Column("item_no", "item_no", tr("Part #"), "varchar", true, 100);                                  // Item's identification number in BL catalog
        columns[2] = Column("item_name", "item_name", tr("Name"), "varchar", true, 300);                                // The name of the item
        columns[3] = Column("item_type", "item_type", tr("Type"), "varchar", false, 80);                                // The type of the item	MINIFIG, PART, SET, BOOK, GEAR, CATALOG, INSTRUCTION, UNSORTED_LOT, ORIGINAL_BOX
        columns[4] = Column("category_name", "category_name", tr("Category"), "varchar", true, 300);                    // [NOT IN API] Name of the category
        columns[5] = Column("color_name", "color_name", tr("Color"), "varchar", true, 200);                             // Color name of the item
        columns[6] = Column("quantity", "quantity", tr("Qty."), "integer", true, 50);                                   // The number of items purchased in this order
        columns[7] = Column("new_or_used", "new_or_used", tr("Cond."), "varchar", true, 50);                            // Indicates whether the item is new or used	N: New, U: Used
        columns[8] = Column("completeness", "completeness", tr("Comp."), "varchar", false, 50);                         // Indicates whether the set is complete or incomplete (This value is valid only for SET type)	C: Complete, B: Incomplete, S: Sealed
        columns[9] = Column("unit_price", "unit_price", tr("Unit Price"), "double", true, 60);                          // The original price of this item per sale unit
        columns[10] = Column("unit_price_final", "unit_price_final", tr("Unit Price Final"), "double", true, 60);       // The unit price of this item after applying tiered pricing policy
        columns[11] = Column("disp_unit_price", "disp_unit_price", tr("User Price"), "double", true, 60);               // The original price of this item per sale unit in display currency of the user
        columns[12] = Column("disp_unit_price_final", "disp_unit_price_final", tr("User Price Final"), "double", true, 60);  // The unit price of this item after applying tiered pricing policy in display currency of the user
        columns[13] = Column("currency_code", "currency_code", tr("Currency"), "varchar", false, 80);                   // The currency code of the price	ISO 4217
        columns[14] = Column("disp_currency_code", "disp_currency_code", tr("User Currency"), "varchar", false, 80);    // The display currency code of the user	ISO 4217
        columns[15] = Column("remarks", "remarks", tr("My Remarks"), "varchar", true, 200);                             // User remarks of the order item
        columns[16] = Column("description", "description", tr("Description"), "varchar", true, 200);                    // User description of the order item
        columns[17] = Column("weight", "weight", tr("Weight"), "double",	true, 100);                                 // The weight of the item that overrides the catalog weight	Upcoming feature
        columns[18] = Column("batchnumber", "batchnumber", tr("Batch"), "integer", false, 50);                          // The number of batch containing the items
        columns[19] = Column("color_id", "color_id", tr("Color ID"), "integer", false, 50);                             // The ID of the color of the item
        columns[20] = Column("category_id", "category_id", tr("Category ID"), "integer", true, 50);                     // The main category of the item
        sortColumn = 2;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::orders:
        sqlTable = "orders";
        columns[0] = Column("order_id", "order_id", tr("ID"), "integer", true, 80);
        columns[1] = Column("date_ordered", "date_ordered", tr("Date"), "varchar", true, 200);
        columns[2] = Column("seller_name", "seller_name", tr("Seller"), "varchar", false, 200);
        columns[3] = Column("store_name", "store_name", tr("Store"), "varchar", false, 200);
        columns[4] = Column("buyer_name", "buyer_name", tr("Buyer"), "varchar", true, 200);
        columns[5] = Column("total_count", "total_count", tr("Qty."), "integer", true, 50);
        columns[6] = Column("unique_count", "unique_count", tr("Unique"), "integer", true, 50);
        columns[7] = Column("status", "status", tr("Status"), "varchar", true, 100);
        columns[8] = Column("payment_method", "payment_method", tr("Pay Method"), "varchar", true, 150);
        columns[9] = Column("payment_status", "payment_status", tr("Pay Status"), "varchar", false, 150);
        columns[10] = Column("payment_date_paid", "payment_date_paid", tr("Pay Date"), "varchar", false, 300);
        columns[11] = Column("payment_currency_code", "payment_currency_code", tr("Pay Currency"), "varchar", false, 100);
        columns[12] = Column("cost_subtotal", "cost_subtotal", tr("Subtotal"), "double", false, 100);
        columns[13] = Column("cost_grand_total", "cost_grand_total", tr("Total"), "double", false, 100);
        columns[14] = Column("cost_currency_code", "cost_currency_code", tr("Cost Currency"), "varchar", false, 150);
        sortColumn = 1;
        sortOrder = Qt::DescendingOrder;
        break;
    case Tables::books:
    case Tables::boxes:
    case Tables::catalogs:
    case Tables::gear:
    case Tables::instructions:
    case Tables::minifigs:
    case Tables::parts:
    case Tables::sets:
        sqlTable = tableName;
        columns[0] = Column("id", "id", tr("ID"), "integer", false, 100);
        columns[1] = Column("item_type", "ITEMTYPE", tr("Type"), "varchar", false, 50);
        columns[2] = Column("item_no", "ITEMID", tr("Part #"), "varchar", true, 100);
        columns[3] = Column("item_name", "ITEMNAME", tr("Name"), "integer", true, 500);
        columns[4] = Column("category_id", "CATEGORY", tr("Category ID"), "integer", false, 80);
        columns[5] = Column("year", "ITEMYEAR", tr("Year"), "integer", false, 80);
        columns[6] = Column("weight", "ITEMWEIGHT", tr("Weight"), "double", false, 80);
        columns[7] = Column("size_x", "ITEMDIMX", tr("Size X"), "integer", false, 50);
        columns[8] = Column("size_y", "ITEMDIMY", tr("Size Y"), "integer", false, 50);
        columns[9] = Column("size_z", "ITEMDIMZ", tr("Size Z"), "integer", false, 50);
        sortColumn = 1;
        sortOrder = Qt::DescendingOrder;
        break;
    case Tables::partcolor:
        sqlTable = "partcolor";
        columns[0] = Column("id", "id", tr("ID"), "integer", false, 100);
        columns[1] = Column("codename", "CODENAME", tr("Codename"), "varchar", false, 50);
        columns[2] = Column("color_name", "COLOR", tr("Color Name"), "varchar", false, 100);
        columns[3] = Column("item_id", "ITEMID", tr("Item ID"), "varchar", false, 500);
        columns[4] = Column("item_type", "ITEMTYPE", tr("Item Type"), "varchar", false, 500);
        sortColumn = 3;
        sortOrder = Qt::DescendingOrder;
        break;
    case Tables::storage:
        sqlTable = "storage";
        columns[0] = Column("id", "id", tr("ID"), "integer", false, 100);
        columns[1] = Column("name", "name", tr("Container name"), "varchar", true, 300);
        columns[2] = Column("sort", "sort", tr("Type"), "varchar", true, 150);
        columns[3] = Column("item_id", "ITEMID", tr("Item ID"), "varchar", true, 250);
        sortColumn = 1;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::userinventories:
        sqlTable = "userinventories";
        columns[0] = Column("id", "inventory_id", tr("ID"), "integer", false, 80);
        columns[1] = Column("item_no", "item_no", tr("Part #"), "varchar", true, 100);
        columns[2] = Column("item_name", "item_name", tr("Name"), "varchar", true, 300);
        columns[3] = Column("item_type", "item_type", tr("Type"), "varchar", false, 80);
        columns[4] = Column("category_name", "category_name", tr("Category"), "integer", true, 300);
        columns[5] = Column("color_name", "color_name", tr("Color"), "varchar", true, 200);
        columns[6] = Column("quantity", "quantity", tr("Qty."), "integer", true, 50);
        columns[7] = Column("new_or_used", "new_or_used", tr("Cond."), "varchar", true, 50);
        columns[8] = Column("completeness", "completeness", tr("Comp."), "varchar", false, 50);
        columns[9] = Column("unit_price", "unit_price", tr("Unit Price"), "double", true, 60);
        columns[10] = Column("bind_id", "bind_id", tr("Bind ID"), "integer", false, 50);
        columns[11] = Column("description", "description", tr("Description"), "varchar", true, 200);
        columns[12] = Column("remarks", "remarks", tr("My Remarks"), "varchar", true, 200);
        columns[13] = Column("bulk", "bulk", tr("Bulk"), "integer", false, 50);
        columns[14] = Column("is_retain", "is_retain", tr("Retain"), "bool", false, 50);
        columns[15] = Column("is_stock_room", "is_stock_room", tr("Stockroom"), "bool", false, 50);
        columns[16] = Column("date_created", "date_created", tr("Date Created"), "datetime", false, 200);
        columns[17] = Column("my_cost", "my_cost", tr("My Cost"), "double", false, 50);
        columns[18] = Column("sale_rate", "sale_rate", tr("Sale"), "integer", true, 50);
        columns[19] = Column("tier_quantity1", "tier_quantity1", tr("Tier Q1"), "integer", false, 50);
        columns[20] = Column("tier_price1", "tier_price1", tr("Tier P1"), "double", false, 50);
        columns[21] = Column("tier_quantity2", "tier_quantity2", tr("Tier Q2"), "integer", false, 50);
        columns[22] = Column("tier_price2", "tier_price2", tr("Tier P2"), "double", false, 50);
        columns[23] = Column("tier_quantity3", "tier_quantity3", tr("Tier Q3"), "integer", false, 50);
        columns[24] = Column("tier_price3", "tier_price3", tr("Tier P3"), "double", false, 50);
        columns[25] = Column("my_weight", "my_weight", tr("My Weight"), "double", false, 50);
        columns[26] = Column("color_id", "color_id", tr("Color ID"), "integer", false, 50);
        columns[27] = Column("category_id", "category_id", tr("Category ID"), "integer", false, 50);
        sortColumn = 2;
        sortOrder = Qt::AscendingOrder;
        break;
    }
}


QString DataModel::getSqlTableName() {
    return sqlTable;
}

int DataModel::getNumberOfColumns() {
    return columns.size();
}
QString DataModel::getSqlColumnName(int column) {
    return columns[column].property.importName;
}
QString DataModel::getSqlColumnName(QString fieldName) {
    QString output = fieldName;
    for(int i = 0; i < columns.size(); i++) {
        if (columns[i].property.name == fieldName)
            return columns[i].property.importName;
    }
    return output;
}
QString DataModel::getColumnHeader(int column) {
    return columns[column].property.displayName;
}
bool DataModel::isColumnVisible(int column) {
    return columns[column].property.visible;
}
void DataModel::setColumnVisible(int column, bool visible) {
    columns[column].property.visible = visible;
}
int DataModel::getColumnWidth(int column) {
    return columns[column].property.columnWidth;
}
int DataModel::getSortColumn() {
    return sortColumn;
}
Qt::SortOrder DataModel::getSortOrder() {
    return sortOrder;
}

QMap<QString, QString> DataModel::getTranslationTable() {
    QMap<QString, QString> output;
    for(int i = 0; i < columns.size(); i++) {
        output[columns[i].property.importName] = columns[i].property.name;
    }
    return output;
}

QSqlError DataModel::initiateSqlTable(QString database) {
    QString queryString;
    queryString = "CREATE TABLE IF NOT EXISTS '" + getSqlTableName() + "'('" + columns[0].property.name + "' " + columns[0].property.sqlType + " PRIMARY KEY" ;
    for(int i = 1; i < getNumberOfColumns(); i++) {
        queryString += ", '" + columns[i].property.name + "' " + columns[i].property.sqlType;
    }
    queryString += ")";
    QSqlQuery q(queryString, QSqlDatabase::database(database));
    if (!q.exec()) {
        qDebug() << "DataModel::initiateSqlTableAuto" << q.lastError();
        return q.lastError();
    }
    return QSqlError();
}

QSqlError DataModel::initiateSqlTableAuto(QString database) {
    QString queryString;
    queryString = "CREATE TABLE IF NOT EXISTS '" + getSqlTableName() + "'(id INTEGER PRIMARY KEY AUTOINCREMENT" ;
    for(int i = 1; i < getNumberOfColumns(); i++) {
        queryString += ", '" + columns[i].property.name + "' " + columns[i].property.sqlType;
    }
    queryString += ")";
    QSqlQuery q(queryString, QSqlDatabase::database(database));
    if (!q.exec()) {
        qDebug() << "DataModel::initiateSqlTableAuto" << q.lastError().text();
        return q.lastError();
    }
    return QSqlError();
}

QSqlError DataModel::truncateSqlTable(QString database) {
    QString queryString = QString("DELETE FROM '%1'").arg(getSqlTableName());
    QSqlQuery q(queryString, QSqlDatabase::database(database));
    if (!q.exec()) {
        qDebug() << "Failed to truncate" << getSqlTableName() << q.lastError();
        return q.lastError();
    }
    return QSqlError();
}

QSqlError DataModel::addItemToTable(QMap<QString, QVariant> fields, QString database)
{
    QString qryString = "INSERT INTO '";
    qryString += this->getSqlTableName() + "'(" + this->columns[0].property.name;
    QString qryStringEnd = "";
    for (int i = 1; i < this->getNumberOfColumns(); i++) {
        if(fields[columns[i].property.importName].isValid()) {
            qryString += ", " + this->columns[i].property.name;
            qryStringEnd += ", ?";
        }
    }
    qryString += ") VALUES(?" + qryStringEnd + ")";
    QSqlQuery q(QSqlDatabase::database(database));
    if (!q.prepare(qryString)) {
        qDebug() << "Failed to add item to database" << getSqlTableName() << q.lastError();
        return q.lastError();
    }
    if (fields[columns[0].property.importName].isValid()) {
        q.addBindValue(fields[columns[0].property.importName]);
    } else {
        itemCounter++;
        q.addBindValue(itemCounter);
    }
    for(int i = 1; i < fields.count(); ++i)
    {
        if(fields[columns[i].property.importName].isValid())
            q.addBindValue(fields[columns[i].property.importName]);
    }
    q.exec();
    return QSqlError();
}
