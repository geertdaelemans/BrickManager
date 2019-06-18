#include "datamodel.h"
#include "sqldatabase.h"
#include "bricklink.h"

SqlDatabase::SqlDatabase()
{
    QSqlError error = initDb();
    if(error.type() != QSqlError::NoError)
        qDebug() << "initDb" << error.text();
}


/**
 * Returns color name by id
 * @param id of color to retrieve
 * @return color name corresponding to id or empty string if no category
 */
QString SqlDatabase::getColorById(int color_id)
{
    QString output;
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare("SELECT color_name FROM colors WHERE id=" + QString::number(color_id)))
        return "Color not found";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    q.finish();
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
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare("SELECT color_code FROM colors WHERE id=" + QString::number(color_id)))
        return "Color not found";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    q.finish();
    return output;
}


/**
 * Returns color ID by name
 * @param name of color to retrieve
 * @return color ID corresponding to id or -1 if no category
 */
int SqlDatabase::getColorIdByName(QString color_name)
{
    int id = -1;
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare(QString("SELECT id FROM colors WHERE color_name='%1'").arg(color_name)))
        return -1;
    q.exec();
    while(q.next()) {
        id = q.value(0).toInt();
    }
    q.finish();
    return id;
}


/**
 * Returns color code by name
 * @param name of color to retrieve
 * @return color code corresponding to id or empty string if no category
 */
QString SqlDatabase::getColorCodeByName(const QString &color_name)
{
    QString output;
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare("SELECT color_code FROM colors WHERE color_name='" + color_name + "'"))
        return "Color not found";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    q.finish();
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
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare("SELECT category_name FROM categories WHERE id=" + QString::number(category_id)))
        return "";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    q.finish();
    return output;
}


/**
 * Returns ID's of all colors a part appears in
 * @param id of part to retrieve
 * @return list of color ID's, empty list when no colors found
 */

QList<int> SqlDatabase::getColorsOfPart(QString item_id)
{
    QList<int> colorList;
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare("SELECT color_name FROM partcolor WHERE item_id=\"" + item_id + "\""))
        return colorList;
    q.exec();
    while (q.next()) {
        colorList.append(getColorIdByName(q.value(0).toString()));
    }
    return colorList;
}


/**
 * Initializes the SQL database and creates the common tables
 * @param none
 * @return QSqlError code
 */
QSqlError SqlDatabase::initDb()
{
//    QTemporaryFile tmpFile(qApp);
//    tmpFile.setFileTemplate("XXXXXX.sqlite3");
//    if (tmpFile.open()) {
//        QString tmp_filename=tmpFile.fileName();
//        qDebug() << "temporary" << tmp_filename;

//        QFile file(":/testdata/catalog_database.db");
//        if (file.open(QIODevice::ReadOnly)) {
//            tmpFile.write(file.readAll());
//        }

//        tmpFile.close();
//    }


    catalogDataBase = QSqlDatabase::addDatabase("QSQLITE", "catalogDatabase");
    tempDataBase = QSqlDatabase::addDatabase("QSQLITE", "tempDatabase");


//    QSqlQuery("PRAGMA page_size = 4096", QSqlDatabase::database("catalogDatabase"));
//    QSqlQuery("PRAGMA cache_size = 16384", QSqlDatabase::database("catalogDatabase"));
//    QSqlQuery("PRAGMA temp_store = MEMORY", QSqlDatabase::database("catalogDatabase"));

    QSqlQuery("PRAGMA journal_mode = OFF", QSqlDatabase::database("catalogDatabase"));
//    QSqlQuery("PRAGMA locking_mode = EXCLUSIVE", QSqlDatabase::database("catalogDatabase"));
    QSqlQuery("PRAGMA synchronous = OFF", QSqlDatabase::database("catalogDatabase"));


    catalogDataBase.setDatabaseName("./database.db");
//    tempDataBase.setDatabaseName("./temp_database.db");
    tempDataBase.setDatabaseName(":memory:");

    if (!catalogDataBase.open())
        qDebug() << "catalogDataBase" << catalogDataBase.lastError();
    if (!tempDataBase.open())
        qDebug() << "tempDataBase" << tempDataBase.lastError();

    QStringList tables = catalogDataBase.tables();
    QStringList tempTables = tempDataBase.tables();

    if (!tables.contains("categories")) {
        DataModel *catModel = new DataModel(Tables::categories);
        catModel->initiateSqlTable("catalogDatabase");
    }

    if (!tables.contains("colors")) {
        DataModel *colModel = new DataModel(Tables::colors);
        colModel->initiateSqlTable("catalogDatabase");
    }

    // Delete previous Order Item tables
    foreach (QString table, tempTables) {
        QSqlQuery q("DROP TABLE IF EXISTS " + table, tempDataBase);
        q.exec();
    }

    if (!tempTables.contains("userinventories")) {
        DataModel *userInvModel = new DataModel(Tables::userinventories);
        userInvModel->initiateSqlTable("tempDatabase");
    }

    return QSqlError();
}

