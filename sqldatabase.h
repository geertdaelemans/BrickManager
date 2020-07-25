#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql>
#include "container.h"

class SqlDatabase
{
public:
    SqlDatabase();
    static QString getColorById(int color_id);
    static QString getColorCodeById(int color_id);
    static QString getColorCodeByName(const QString &color_name);
    static QString getCategoryById(int category_id);
    static int getColorIdByName(QString category_name);
    static QList<int> getColorsOfPart(QString item_id);
    static QString getUniqueTableName(QString* name, QString sqlTableName = "");
    static QString getTableName(QString name);
    static void updateTableName(QString oldName, QString newName);
    static void removeTable(QString name);
    static void deleteLabel(const int row);
    static void clearAllLabels();
    static void clearTable(QString tableName);
    static QList<Container> getLabels(const QString tableName);
    static void importLabels(QList<Container> containers);
    static QString getContainerLabel(const QString itemID);
    static void updateLabels(const QString table);
    static bool containerListed(Container container);
    static QJsonArray getImagesJson(QString sqlTableName);

private:
    QSqlError initDb();
    QSqlDatabase catalogDataBase;
    QSqlDatabase tempDataBase;
};

#endif // SQLDATABASE_H
