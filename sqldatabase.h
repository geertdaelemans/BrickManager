#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql>

class SqlDatabase
{
public:
    SqlDatabase();
    static QString getColorById(int color_id);
    static QString getColorCodeById(int color_id);
    static QString getColorCodeByName(const QString &color_name);
    static QString getCategoryById(int category_id);

private:
    QSqlError initDb();
    QSqlDatabase catalogDataBase;
    QSqlDatabase tempDataBase;
};

#endif // SQLDATABASE_H
