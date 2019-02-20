#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QtCore>
#include <QTranslator>
#include <QSortFilterProxyModel>

#include "sqldatabase.h"

class Column
{
public:
    struct Properties {
        QString sqlName;
        QString name;
        QString sqlType;
        bool visible;
        int columnWidth;
    };
    Properties property;
    Column() {}
    Column(QString sqlName, QString name, QString sqlType = "varchar", bool visible = false, int columnWidth = 20) {
        property.sqlName = sqlName;
        property.name = name;
        property.sqlType = sqlType;
        property.visible = visible;
        property.columnWidth = columnWidth;
    }

};

enum Tables {
    categories,
    colors,
    brickstock,
    orderitem,
    orders,
    userinventories
};

class TableModel : public QObject
{
        Q_OBJECT

public:
    TableModel(Tables table, QString tableName = "");
    QString getSqlTableName();
    int getNumberOfColumns();
    QString getSqlColumnName(int column);
    QString getColumnHeader(int column);
    bool isColumnVisible(int column);
    void setColumnVisible(int column, bool visible);
    int getColumnWidth(int column);
    int getSortColumn();
    Qt::SortOrder getSortOrder();
    QSqlError initiateSqlTable();
    QSqlError initiateSqlTableAuto();
    QSqlError truncateSqlTable();
    QSqlError addItemToTable(QMap<QString, QVariant> fields);


private:
    QString sqlTable;
    int sortColumn = 0;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;
    QMap<int, Column> columns;
    int itemCounter = 0;
};

#endif // DATAMODELS_H
