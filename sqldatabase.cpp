#include "datamodels.h"

SqlDatabase::SqlDatabase()
{
    QSqlError error = initDb();
    if(error.type() != QSqlError::NoError)
        qDebug() << error.text();
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
//    db.setDatabaseName("./database.db");
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();

    TableModel *catModel = new TableModel(Tables::categories);
    catModel->initiateSqlTable();
    TableModel *colModel = new TableModel(Tables::colors);
    colModel->initiateSqlTable();
    TableModel *userInvModel = new TableModel(Tables::userinventories);
    userInvModel->initiateSqlTable();

    return QSqlError();
}
