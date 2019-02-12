#include "sqldatabase.h"
#include "datamodels.h"

SqlDatabase::SqlDatabase()
{
    QSqlError error = initDb();
    if(error.type() != QSqlError::NoError)
        qDebug() << error.text();
}


QSqlError SqlDatabase::addCategory(QList<QVariant> fields)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into categories(category_id, category_name, parent_id) values(?, ?, ?)")))
        return q.lastError();
    for(int i = 0; i < fields.count(); ++i)
    {
        q.addBindValue(fields[i]);
    }
    q.exec();
    return q.lastError();
}


QSqlError SqlDatabase::addColor(QList<QVariant> fields)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into colors(color_id, color_name, color_code, color_type) values(?, ?, ?, ?)")))
        return q.lastError();
    for(int i = 0; i < fields.count(); ++i)
    {
        q.addBindValue(fields[i]);
    }
    q.exec();
    return q.lastError();
}


QSqlError SqlDatabase::addOrderItem(QList<QVariant> fields, int orderID)
{
    QSqlQuery q;
    QString queryString = "insert into orderitem" + QString::number(orderID) + "(inventory_id, item_no, item_name, item_type, category_id, color_id, quantity, new_or_used, completeness, unit_price, unit_price_final, disp_unit_price, disp_unit_price_final, currency_code, disp_currency_code, remarks, description, weight, batchNumber) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    if (!q.prepare(queryString))
        return q.lastError();
    for(int i = 0; i < fields.count(); ++i)
    {
        q.addBindValue(fields[i]);
    }
    q.exec();
    return q.lastError();
}


QSqlError SqlDatabase::addUserInventory(QList<QVariant> fields)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into userinventories(inventory_id, item_no, item_name, item_type, category_id, color_id, quantity, new_or_used, completeness, unit_price, bind_id, description, remarks, bulk, is_retain, is_stock_room, date_created, my_cost, sale_rate, tier_quantity1, tier_quantity2, tier_quantity3, tier_price1, tier_price2, tier_price3, my_weight) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)")))
        return q.lastError();
    for(int i = 0; i < fields.count(); ++i)
    {
        q.addBindValue(fields[i]);
    }
    q.exec();
    return q.lastError();
}

QSqlError SqlDatabase::initiateOrderItemTable(int orderID)
{
    QSqlQuery q;
    TableModel *model = new TableModel(Tables::orderitem);
    model->setSqlTableName("orderitem" + QString::number(orderID));
    if (!q.exec(model->getInitiateSqlDatabaseQry()))
        return q.lastError();
    return QSqlError();
}


/**
 * Returns color name by id
 * @param id of color to retrieve
 * @return color name corresponding to id or empty string if no category
 */
QString SqlDatabase::getColorById(int color_id)
{
    QString output;
    QSqlQuery q;
    if (!q.prepare("SELECT color_name FROM colors WHERE color_id=" + QString::number(color_id)))
        return "Color not found";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    return output;
}


/**
 * Returns color code by id
 * @param id of color to retrieve
 * @return color code corresponding to id or empty string if no category
 */
QString SqlDatabase::getColorCodeById(int color_id)
{
    QString output;
    QSqlQuery q;
    if (!q.prepare("SELECT color_code FROM colors WHERE color_id=" + QString::number(color_id)))
        return "Color not found";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    return output;
}


/**
 * Returns color code by name
 * @param name of color to retrieve
 * @return color code corresponding to id or empty string if no category
 */
QString SqlDatabase::getColorCodeByName(const QString &color_name)
{
    QString output;
    QSqlQuery q;
    if (!q.prepare("SELECT color_code FROM colors WHERE color_name='" + color_name + "'"))
        return "Color not found";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    return output;
}


/**
 * Returns category name by id
 * @param id of category to retrieve
 * @return category name corresponding to id or empty string if no category
 */
QString SqlDatabase::getCategoryById(int category_id)
{
    QString output;
    QSqlQuery q;
    if (!q.prepare("SELECT category_name FROM categories WHERE category_id=" + QString::number(category_id)))
        return "";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    return output;
}


/**
 * Initializes the SQL database and creates the common tables
 * @param none
 * @return QSqlError code
 */
QSqlError SqlDatabase::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./database.db");
//    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();

    QSqlQuery q;

    if (!tables.contains("colors", Qt::CaseInsensitive)) {
        TableModel *model = new TableModel(Tables::colors);
        if (!q.exec(model->getInitiateSqlDatabaseQry()))
            return q.lastError();
    }
    if (!tables.contains("categories", Qt::CaseInsensitive)) {
        TableModel *model = new TableModel(Tables::categories);
        if (!q.exec(model->getInitiateSqlDatabaseQry()))
            return q.lastError();
    }
    if (!tables.contains("userinventories", Qt::CaseInsensitive)) {
        TableModel *model = new TableModel(Tables::userinventories);
        if (!q.exec(model->getInitiateSqlDatabaseQry()))
            return q.lastError();
    }
    if (!tables.contains("orders", Qt::CaseInsensitive)) {
        TableModel *model = new TableModel(Tables::orders);
        if (!q.exec(model->getInitiateSqlDatabaseQry()))
            return q.lastError();
    }
    return QSqlError();
}
