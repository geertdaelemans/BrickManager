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

QSqlError SqlDatabase::initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("d:/database.db");

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

    qDebug() << "Database initialized...";

    return QSqlError();
}
