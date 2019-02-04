#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql>

class SqlDatabase
{
public:
    SqlDatabase();
    static QSqlError addColor(int color_id, const QString &color_name, const QString &color_code, const QString &color_type);
    static QSqlError addCategory(int category_id, const QString &name, int parent_id);
    static QString getColorById(int color_id);
    static QString getCategoryById(int category_id);

private:
    QSqlError initDb();
};

#endif // SQLDATABASE_H
