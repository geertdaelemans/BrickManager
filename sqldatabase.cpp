#include "sqldatabase.h"

SqlDatabase::SqlDatabase()
{
    QSqlError error = initDb();
    if(error.type() != QSqlError::NoError)
        qDebug() << error.text();
}

QSqlError SqlDatabase::addColor(int color_id, const QString &color_name, const QString &color_code, const QString &color_type)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into colors(color_id, color_name, color_code, color_type) values(?, ?, ?, ?)")))
        return q.lastError();
    q.addBindValue(color_id);
    q.addBindValue(color_name);
    q.addBindValue(color_code);
    q.addBindValue(color_type);
    q.exec();
    return q.lastError();
}

QSqlError SqlDatabase::addCategory(int category_id, const QString &category_name, int parent_id)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into categories(category_id, category_name, parent_id) values(?, ?, ?)")))
        return q.lastError();
    q.addBindValue(category_id);
    q.addBindValue(category_name);
    q.addBindValue(parent_id);
    q.exec();
    return q.lastError();
}

QSqlError SqlDatabase::addUserInventory(int inventory_id, const QString &item_no, const QString &item_name, const QString &item_type, int item_category_id,
                              int color_id, const QString &color_name, int quantity, const QString &new_or_used, const QString &completeness,
                              double unit_price, int bind_id, const QString &description, const QString &remarks, int bulk, bool is_retain,
                              bool is_stock_room, QDateTime date_created, double my_cost, int sale_rate, int tier_quantity1,
                              int tier_quantity2, int tier_quantity3, double tier_price1, double tier_price2, double tier_price3,
                              double my_weight)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into userinventories(inventory_id, item_no, item_name, item_type, item_category_id, color_id, color_name, quantity, new_or_used, completeness, unit_price, bind_id, description, remarks, bulk, is_retain, is_stock_room, date_created, my_cost, sale_rate, tier_quantity1, tier_quantity2, tier_quantity3, tier_price1, tier_price2, tier_price3, my_weight) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)")))
        return q.lastError();
    q.addBindValue(inventory_id);
    q.addBindValue(item_no);
    q.addBindValue(item_name);
    q.addBindValue(item_type);
    q.addBindValue(item_category_id);
    q.addBindValue(color_id);
    q.addBindValue(color_name);
    q.addBindValue(quantity);
    q.addBindValue(new_or_used);
    q.addBindValue(completeness);
    q.addBindValue(unit_price);
    q.addBindValue(bind_id);
    q.addBindValue(description);
    q.addBindValue(remarks);
    q.addBindValue(bulk);
    q.addBindValue(is_retain);
    q.addBindValue(is_stock_room);
    q.addBindValue(date_created);
    q.addBindValue(my_cost);
    q.addBindValue(sale_rate);
    q.addBindValue(tier_quantity1);
    q.addBindValue(tier_quantity2);
    q.addBindValue(tier_quantity3);
    q.addBindValue(tier_price1);
    q.addBindValue(tier_price2);
    q.addBindValue(tier_price3);
    q.addBindValue(my_weight);
    q.exec();
    return q.lastError();
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
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("./database.db");
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();

    QSqlQuery q;

    if (!tables.contains("colors", Qt::CaseInsensitive))
        if (!q.exec(QLatin1String("create table colors(color_id integer primary key, color_name varchar, color_code varchar, color_type varchar)")))
            return q.lastError();
    if (!tables.contains("categories", Qt::CaseInsensitive))
        if (!q.exec(QLatin1String("create table categories(category_id integer primary key, category_name varchar, parent_id integer)")))
            return q.lastError();
    if (!tables.contains("userinventories", Qt::CaseInsensitive))
        if (!q.exec(QLatin1String("create table userinventories(inventory_id integer primary key, item_no varchar, item_name varchar, item_type varchar, item_category_id integer, color_id integer, color_name varchar, quantity integer, new_or_used varchar, completeness varchar, unit_price varchar, bind_id integer, description varchar, remarks varchar, bulk integer, is_retain varchar, is_stock_room varchar, date_created varchar, my_cost varchar, sale_rate integer, tier_quantity1 integer, tier_quantity2 integer, tier_quantity3 integer, tier_price1 varchar, tier_price2 varchar, tier_price3 varchar, my_weight varchar)")))
            return q.lastError();


    qDebug() << "Database initialized...";

    return QSqlError();
}
