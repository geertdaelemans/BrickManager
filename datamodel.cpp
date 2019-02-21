#include "datamodel.h"


DataModel::DataModel(Tables table, QString tableName)
{
    switch (table)
    {
    case Tables::brickstock:
        sqlTable = tableName;
        columns[0] = Column("id", tr("ID"), "integer", false, 100);
        columns[1] = Column("Status", tr("Status"), "varchar", true, 21);
        columns[2] = Column("Image", tr("Image"), "varchar", true, 45);
        columns[3] = Column("ItemID", tr("Part #"), "varchar", true, 65);
        columns[4] = Column("ItemName", tr("Description"), "varchar", true, 173);
        columns[5] = Column("Condition", tr("Cond."), "varchar", true, 35);
        columns[6] = Column("ColorID", tr("Color"), "integer", true, 95);
        columns[7] = Column("Qty", tr("Qty."), "integer", true, 35);
        columns[8] = Column("Price", tr("Price"), "double", true, 53);
        columns[9] = Column("Total", tr("Total"), "double", true, 53);
        columns[10] = Column("Bulk", tr("Bulk"), "integer", true, 35);
        columns[11] = Column("Sale", tr("Sale"), "integer", true, 35);
        columns[12] = Column("Comments", tr("Comments"), "varchar", true, 53);
        columns[13] = Column("Remarks", tr("Remarks"), "varchar", true, 53);
        columns[14] = Column("CategoryID", tr("Category"), "integer", true, 77);
        columns[15] = Column("ItemTypeName", tr("Item Type"), "varchar", true, 77);
        columns[16] = Column("TQ1", tr("Tier Q1"), "integer", true, 35);
        columns[17] = Column("TP1", tr("Tier P1"), "double", true, 53);
        columns[18] = Column("TQ2", tr("Tier Q2"), "integer", true, 35);
        columns[19] = Column("TP2", tr("Tier P2"), "double", true, 53);
        columns[20] = Column("TQ3", tr("Tier Q3"), "integer", true, 35);
        columns[21] = Column("TP3", tr("Tier P3"), "double", true, 53);
        columns[22] = Column("LotID", tr("Lot ID"), "integer", true, 21);
        columns[23] = Column("Retain", tr("Retain"), "bool", true, 21);
        columns[24] = Column("Stockroom", tr("Stockroom"), "bool", true, 21);
        columns[25] = Column("Reserved", tr("Reserved"), "varchar", true, 21);
        columns[26] = Column("TotalWeight", tr("Weight"), "double", true, 21);
        columns[27] = Column("Year", tr("Year"), "integer", true, 21);
        columns[28] = Column("OrigQty", tr("Qty. Orig"), "integer", true, 35);
        columns[29] = Column("OrigQtyDiff", tr("Qty. Diff"), "integer", true, 35);
        columns[30] = Column("OrigPrice", tr("Pr. Orig"), "double", true, 53);
        columns[31] = Column("OrigPriceDiff", tr("Pr. Diff"), "double", true, 53);
        columns[32] = Column("CategoryName", tr("Category Name"), "varchar", false, 77);    // Extra's should be ignored
        columns[33] = Column("ColorName", tr("Color Name"), "varchar", false, 95);          // Extra's should be ignored
        columns[34] = Column("ItemTypeID", tr("Item Type ID"), "varchar", false, 35);       // Extra's should be ignored
        sortColumn = 2;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::categories:
        sqlTable = "categories";
        columns[0] = Column("category_id", tr("ID"), "integer", false, 50);
        columns[1] = Column("category_name", tr("Name"), "varchar", true, 300);
        columns[2] = Column("parent_id", tr("Parent"), "integer", false, 80);
        sortColumn = 1;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::colors:
        sqlTable = "colors";
        columns[0] = Column("color_id", tr("ID"), "integer", false, 50);
        columns[1] = Column("color_name", tr("Name"), "varchar", true, 200);
        columns[2] = Column("color_code", tr("Code"), "varchar", false, 80);
        columns[3] = Column("color_type", tr("Type"), "varchar", true, 100);
        sortColumn = 1;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::orderitem:
        sqlTable = "orderitem" + tableName;
        columns[0] = Column("inventory_id", tr("ID"), "integer", false, 80);                        // The ID of the inventory that includes the item
        columns[1] = Column("item_no", tr("Part #"), "varchar", true, 100);                         // Item's identification number in BL catalog
        columns[2] = Column("item_name", tr("Name"), "varchar", true, 300);                         // The name of the item
        columns[3] = Column("item_type", tr("Type"), "varchar", false, 80);                         // The type of the item	MINIFIG, PART, SET, BOOK, GEAR, CATALOG, INSTRUCTION, UNSORTED_LOT, ORIGINAL_BOX
        columns[4] = Column("category_id", tr("Category"), "integer", true, 300);                   // The main category of the item
        columns[5] = Column("color_id", tr("Color"), "integer", true, 200);                         // The ID of the color of the item
        columns[6] = Column("quantity", tr("Qty."), "integer", true, 50);                           // The number of items purchased in this order
        columns[7] = Column("new_or_used", tr("Cond."), "varchar", true, 50);                       // Indicates whether the item is new or used	N: New, U: Used
        columns[8] = Column("completeness", tr("Comp."), "varchar", false, 50);                     // Indicates whether the set is complete or incomplete (This value is valid only for SET type)	C: Complete, B: Incomplete, S: Sealed
        columns[9] = Column("unit_price", tr("Unit Price"), "double", true, 60);                    // The original price of this item per sale unit
        columns[10] = Column("unit_price_final", tr("Unit Price Final"), "double", true, 60);       // The unit price of this item after applying tiered pricing policy
        columns[11] = Column("disp_unit_price", tr("User Price"), "double", true, 60);              // The original price of this item per sale unit in display currency of the user
        columns[12] = Column("disp_unit_price_final", tr("User Price Final"), "double", true, 60);  // The unit price of this item after applying tiered pricing policy in display currency of the user
        columns[13] = Column("currency_code", tr("Currency"), "varchar", false, 80);                // The currency code of the price	ISO 4217
        columns[14] = Column("disp_currency_code", tr("User Currency"), "varchar", false, 80);      // The display currency code of the user	ISO 4217
        columns[15] = Column("remarks", tr("My Remarks"), "varchar", true, 200);                    // User remarks of the order item
        columns[16] = Column("description", tr("Description"), "varchar", true, 200);               // User description of the order item
        columns[17] = Column("weight", tr("Weight"), "double",	true, 100);                         // The weight of the item that overrides the catalog weight	Upcoming feature
        columns[18] = Column("batchnumber", tr("Batch"), "integer", false, 50);                     // The number of batch containing the items
        sortColumn = 2;
        sortOrder = Qt::AscendingOrder;
        break;
    case Tables::orders:
        sqlTable = "orders";
        columns[0] = Column("order_id", tr("ID"), "integer", true, 80);
        columns[1] = Column("date_ordered", tr("Date"), "varchar", true, 200);
        columns[2] = Column("seller_name", tr("Seller"), "varchar", false, 200);
        columns[3] = Column("store_name", tr("Store"), "varchar", false, 200);
        columns[4] = Column("buyer_name", tr("Buyer"), "varchar", true, 200);
        columns[5] = Column("total_count", tr("Qty."), "integer", true, 50);
        columns[6] = Column("unique_count", tr("Unique"), "integer", true, 50);
        columns[7] = Column("status", tr("Status"), "varchar", true, 100);
        columns[8] = Column("payment_method", tr("Pay Method"), "varchar", true, 150);
        columns[9] = Column("payment_status", tr("Pay Status"), "varchar", false, 150);
        columns[10] = Column("payment_date_paid", tr("Pay Date"), "varchar", false, 300);
        columns[11] = Column("payment_currency_code", tr("Pay Currency"), "varchar", false, 100);
        columns[12] = Column("cost_subtotal", tr("Subtotal"), "double", false, 100);
        columns[13] = Column("cost_grand_total", tr("Total"), "double", false, 100);
        columns[14] = Column("cost_currency_code", tr("Cost Currency"), "varchar", false, 150);
        sortColumn = 1;
        sortOrder = Qt::DescendingOrder;
        break;
    case Tables::userinventories:
        sqlTable = "userinventories";
        columns[0] = Column("inventory_id", tr("ID"), "integer", false, 80);
        columns[1] = Column("item_no", tr("Part #"), "varchar", true, 100);
        columns[2] = Column("item_name", tr("Name"), "varchar", true, 300);
        columns[3] = Column("item_type", tr("Type"), "varchar", false, 80);
        columns[4] = Column("category_id", tr("Category"), "integer", true, 300);
        columns[5] = Column("color_id", tr("Color"), "integer", true, 200);
        columns[6] = Column("quantity", tr("Qty."), "integer", true, 50);
        columns[7] = Column("new_or_used", tr("Cond."), "varchar", true, 50);
        columns[8] = Column("completeness", tr("Comp."), "varchar", false, 50);
        columns[9] = Column("unit_price", tr("Unit Price"), "double", true, 60);
        columns[10] = Column("bind_id", tr("Bind ID"), "integer", false, 50);
        columns[11] = Column("description", tr("Description"), "varchar", true, 200);
        columns[12] = Column("remarks", tr("My Remarks"), "varchar", true, 200);
        columns[13] = Column("bulk", tr("Bulk"), "integer", false, 50);
        columns[14] = Column("is_retain", tr("Retain"), "bool", false, 50);
        columns[15] = Column("is_stock_room", tr("Stockroom"), "bool", false, 50);
        columns[16] = Column("date_created", tr("Date Created"), "datetime", false, 200);
        columns[17] = Column("my_cost", tr("My Cost"), "double", false, 50);
        columns[18] = Column("sale_rate", tr("Sale"), "integer", true, 50);
        columns[19] = Column("tier_quantity1", tr("Tier Q1"), "integer", false, 50);
        columns[20] = Column("tier_price1", tr("Tier P1"), "double", false, 50);
        columns[21] = Column("tier_quantity2", tr("Tier Q2"), "integer", false, 50);
        columns[22] = Column("tier_price2", tr("Tier P2"), "double", false, 50);
        columns[23] = Column("tier_quantity3", tr("Tier Q3"), "integer", false, 50);
        columns[24] = Column("tier_price3", tr("Tier P3"), "double", false, 50);
        columns[25] = Column("my_weight", tr("My Weight"), "double", false, 50);
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
    return columns[column].property.sqlName;
}
QString DataModel::getColumnHeader(int column) {
    return columns[column].property.name;
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

QSqlError DataModel::initiateSqlTable() {
    QString queryString;
    queryString = "CREATE TABLE " + getSqlTableName() + "(" + columns[0].property.sqlName + " " + columns[0].property.sqlType + " PRIMARY KEY" ;
    for(int i = 1; i < getNumberOfColumns(); i++) {
        queryString += ", " + columns[i].property.sqlName + " " + columns[i].property.sqlType;
    }
    queryString += ")";
    QSqlQuery q;
    if (!q.exec(queryString))
        return q.lastError();
    return QSqlError();
}

QSqlError DataModel::initiateSqlTableAuto() {
    QString queryString;
    queryString = "CREATE TABLE " + getSqlTableName() + "(id INTEGER PRIMARY KEY AUTOINCREMENT" ;
    for(int i = 1; i < getNumberOfColumns(); i++) {
        queryString += ", " + columns[i].property.sqlName + " " + columns[i].property.sqlType;
    }
    queryString += ")";
    QSqlQuery q;
    if (!q.exec(queryString))
        return q.lastError();
    return QSqlError();
}

QSqlError DataModel::truncateSqlTable() {
    QString queryString = "DELETE FROM " + getSqlTableName();
    QSqlQuery q;
    if (!q.exec(queryString)) {
        qDebug() << "Failed to truncate" << getSqlTableName() << q.lastError();
        return q.lastError();
    }
    return QSqlError();
}

QSqlError DataModel::addItemToTable(QMap<QString, QVariant> fields)
{
    QString qryString = "INSERT INTO ";
    qryString += this->getSqlTableName() + "(" + this->columns[0].property.sqlName;
    QString qryStringEnd = "";
    for (int i = 1; i < this->getNumberOfColumns(); i++) {
        if(fields[columns[i].property.sqlName].isValid()) {
            qryString += ", " + this->columns[i].property.sqlName;
            qryStringEnd += ", ?";
        }
    }
    qryString += ") VALUES(?" + qryStringEnd + ")";
    QSqlQuery q;
    if (!q.prepare(qryString)) {
        qDebug() << "Failed to add item to database" << getSqlTableName() << q.lastError();
        return q.lastError();
    }
    if (fields[columns[0].property.sqlName].isValid()) {
        q.addBindValue(fields[columns[0].property.sqlName]);
    } else {
        itemCounter++;
        q.addBindValue(itemCounter);
    }
    for(int i = 1; i < fields.count(); ++i)
    {
        if(fields[columns[i].property.sqlName].isValid())
            q.addBindValue(fields[columns[i].property.sqlName]);
    }
    q.exec();
    return QSqlError();
}
