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
    QSqlQuery query(QSqlDatabase::database("catalogDatabase"));
    if (!query.prepare("SELECT color_name FROM partcolor WHERE item_id='" + item_id + "'"))
        return colorList;
    query.exec();
    while (query.next()) {
        colorList.append(getColorIdByName(query.value(0).toString()));
    }
    return colorList;
}

QList<Container> SqlDatabase::getLabels(const QString tableName)
{
    QList<Container> containerList;
    QSqlQuery query(QSqlDatabase::database("tempDatabase"));
    if (!query.prepare("SELECT DISTINCT remarks, item_no FROM " + tableName + " WHERE remarks != ''"))
        return containerList;
    query.exec();
    while (query.next()) {
        containerList.append(Container(query.value(0).toString(), query.value(1).toString()));
    }
    return containerList;
}

void SqlDatabase::importLabels(QList<Container> containers)
{
    QSqlDatabase::database("catalogDatabase").transaction();
    for (int i = 0; i < containers.length(); i++) {
        QString queryString = QString("REPLACE INTO storage (name, sort, item_id) VALUES (:name, :sort, :item_id)");
        QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
        if (!q.prepare(queryString))
            qDebug() << "SqlDatabase::importLabels" << q.lastError() << queryString;
        q.bindValue(":name", containers[i].getName());
        q.bindValue(":sort", containers[i].getName().split(" ").at(0));
        q.bindValue(":item_id", containers[i].getItemID());
        q.exec();
    }
    QSqlDatabase::database("catalogDatabase").commit();
}

QString SqlDatabase::getContainerLabel(const QString item_id)
{
    QString output;
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare("SELECT name FROM storage WHERE item_id='" + item_id + "'"))
        return "";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();;
    }
    q.finish();
    return output;
}

void SqlDatabase::clearAllLabels()
{
    QSqlQuery q(QSqlDatabase::database("catalogDatabase"));
    if (!q.prepare(QString("DROP TABLE IF EXISTS storage")))
        qDebug() << QString("Problem dropping storage table");
    q.exec();
    q.first();
    q.finish();
    DataModel *storageModel = new DataModel(Tables::storage);
    storageModel->initiateSqlTableAuto("catalogDatabase");
}

QString SqlDatabase::getUniqueTableName(QString* name, QString sqlTableName)
{
    QString uniqueName = *name;
    QSqlQuery q(QSqlDatabase::database("tempDatabase"));
    int counter = 1;
    bool found = true;
    while(found) {
        QString query = QString("SELECT EXISTS(SELECT 1 FROM indextable WHERE name='%1')").arg(uniqueName);
        q.exec(query);
        while(q.next())
            found = q.value(0).toBool();
        if (found) {
            uniqueName = QString("%1 (%2)").arg(*name).arg(counter);
            counter++;
        }
    }
    *name = uniqueName;
    if (sqlTableName == "") {
        q.exec(QString("SELECT MAX(id) FROM indextable"));
        int maxIndex = 0;
        while(q.next())
            maxIndex = q.value(0).toInt();
        sqlTableName = QString("table_%1").arg(maxIndex+1);
    }

    QString qryString = QString("INSERT INTO 'indextable' ('name', 'sqltable') VALUES ('%1', '%2')").arg(uniqueName).arg(sqlTableName);

    if (!q.exec(qryString)) {
        qDebug() << "Failed to add item to database" << uniqueName << q.lastError();
        qDebug() << q.lastError();
    }

    return sqlTableName;
}


QString SqlDatabase::getTableName(QString name)
{
    QString output;
    QSqlQuery q(QSqlDatabase::database("tempDatabase"));
    if (!q.prepare(QString("SELECT sqltable FROM indextable WHERE name='%1'").arg(name)))
        return "";
    q.exec();
    while (q.next()) {
        output = q.value(0).toString();
    }
    q.finish();
    return output;
}


void SqlDatabase::updateTableName(QString oldName, QString newName)
{
    QSqlQuery q(QSqlDatabase::database("tempDatabase"));
    if (!q.prepare(QString("UPDATE indextable SET name='%1' WHERE name='%2'").arg(newName).arg(oldName)))
        qDebug() << "Error updating table name";
    q.exec();
    q.finish();
}


void SqlDatabase::removeTable(QString name)
{
    QString sqlTableName = getTableName(name);
    QSqlQuery q(QSqlDatabase::database("tempDatabase"));
    if (!q.prepare(QString("DELETE FROM indextable WHERE name='%1'").arg(name)))
        qDebug() << "Problem deleting row from indextable";
    q.exec();
    q.first();
    if (!q.prepare(QString("DROP TABLE %1").arg(sqlTableName)))
        qDebug() << QString("Problem dropping table %1").arg(sqlTableName);
    q.exec();
    q.first();
    q.finish();
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

    if (!tables.contains("storage")) {
        DataModel *storageModel = new DataModel(Tables::storage);
        storageModel->initiateSqlTableAuto("catalogDatabase");
    }

    if (!tables.contains("indextable")) {
        DataModel *indexModel = new DataModel(Tables::indextable);
        indexModel->initiateSqlTableAuto("tempDatabase");
    }

    // Delete previous Order Item tables
    foreach (QString table, tempTables) {
        QSqlQuery q(QString("DROP TABLE IF EXISTS '%1'").arg(table), tempDataBase);
        q.exec();
    }

    if (!tempTables.contains("userinventories")) {
        DataModel *userInvModel = new DataModel(Tables::userinventories);
        userInvModel->initiateSqlTable("tempDatabase");
    }

    return QSqlError();
}

