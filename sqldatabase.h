#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql>

class SqlDatabase
{
public:
    SqlDatabase();
    static QSqlError addCategory(QList<QVariant> fields);
    static QSqlError addColor(QList<QVariant> fields);
    static QSqlError addOrderItem(QList<QVariant> fields, int orderID);
    static QSqlError addUserInventory(QList<QVariant> fields);
    static QString getColorById(int color_id);
    static QString getColorCodeById(int color_id);
    static QString getColorCodeByName(const QString &color_name);
    static QString getCategoryById(int category_id);
    static QSqlError initiateOrderItemTable(int orderID);

private:
    QSqlDatabase db;
    QSqlError initDb();
};

#endif // SQLDATABASE_H
