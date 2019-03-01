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
        QString name;
        QString importName;
        QString displayName;
        QString sqlType;
        bool visible;
        int columnWidth;
    };
    Properties property;
    Column() {}
    Column(QString name, QString sqlName, QString displayName, QString sqlType = "varchar", bool visible = false, int columnWidth = 20) {
        property.name = name;
        property.importName = sqlName;
        property.displayName = displayName;
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
    parts,
    userinventories
};

class DataModel : public QObject
{
        Q_OBJECT

public:
    DataModel(Tables table, QString tableName = "");
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
