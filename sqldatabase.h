#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql>

class SqlDatabase
{
public:
    SqlDatabase();
    static QSqlError addColor(int color_id, const QString &color_name, const QString &color_code, const QString &color_type);
    static QSqlError addCategory(int category_id, const QString &name, int parent_id);
    static QSqlError addUserInventory(
            int inventory_id,
            const QString &item_no,
            const QString &item_name,
            const QString &item_type,
            int item_category_id,
            int color_id,
            const QString &color_name,
            int quantity,
            const QString &new_or_used,
            const QString &completeness,
            double unit_price,
            int bind_id,
            const QString &description,
            const QString &remarks,
            int bulk,
            bool is_retain,
            bool is_stock_room,
            QDateTime date_created,
            double my_cost,
            int sale_rate,
            int tier_quantity1,
            int tier_quantity2,
            int tier_quantity3,
            double tier_price1,
            double tier_price2,
            double tier_price3,
            double my_weight);
    static QString getColorById(int color_id);
    static QString getCategoryById(int category_id);


private:
    QSqlError initDb();
};

#endif // SQLDATABASE_H
