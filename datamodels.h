#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QtCore>
#include <QTranslator>
#include <QSortFilterProxyModel>

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
    userinventories
};

class TableModel : public QObject
{
        Q_OBJECT

public:
    TableModel(Tables table)
    {
        switch (table)
        {
        case Tables::categories:
            sqlTable = "categories";
            columns[0] = Column("category_id", tr("ID"), "integer", false, 50);
            columns[1] = Column("category_name", tr("Name"), "varchar", true, 300);
            columns[2] = Column("parent_id", tr("Parent"), "integer", false, 80);
            sortColumn = 1;
            sortOrder = Qt::AscendingOrder;
            break;
        case Tables::colors:
            sqlTable = "colors";
            columns[0] = Column("color_id", tr("ID"), "integer", false, 50);
            columns[1] = Column("color_name", tr("Name"), "varchar", true, 200);
            columns[2] = Column("color_code", tr("Code"), "varchar", false, 80);
            columns[3] = Column("color_type", tr("Type"), "varchar", true, 100);
            sortColumn = 1;
            sortOrder = Qt::AscendingOrder;
            break;
        case Tables::userinventories:
            sqlTable = "userinventories";
            columns[0] = Column("inventory_id", tr("ID"), "integer", false, 80);
            columns[1] = Column("item_no", tr("Part #"), "varchar", true, 100);
            columns[2] = Column("item_name", tr("Name"), "varchar", true, 300);
            columns[3] = Column("item_type", tr("Type"), "varchar", false, 80);
            columns[4] = Column("item_category_id", tr("Category ID"), "integer", false, 50);
            columns[5] = Column("color_id", tr("Color ID"), "integer", false, 50);
            columns[6] = Column("color_name", tr("Color"), "varchar", true, 150);
            columns[7] = Column("quantity", tr("Qty."), "integer", true, 50);
            columns[8] = Column("new_or_used", tr("Cond."), "varchar", true, 50);
            columns[9] = Column("completeness", tr("Comp."), "varchar", false, 50);
            columns[10] = Column("unit_price", tr("Unit Price"), "double", true, 60);
            columns[11] = Column("bind_id", tr("Bind ID"), "integer", false, 50);
            columns[12] = Column("description", tr("Description"), "varchar", true, 200);
            columns[13] = Column("remarks", tr("My Remarks"), "varchar", true, 200);
            columns[14] = Column("bulk", tr("Bulk"), "integer", false, 50);
            columns[15] = Column("is_retain", tr("Retain"), "bool", false, 50);
            columns[16] = Column("is_stock_room", tr("Stockroom"), "bool", false, 50);
            columns[17] = Column("date_created", tr("Date Created"), "date", false, 200);
            columns[18] = Column("my_cost", tr("My Cost"), "double", false, 50);
            columns[19] = Column("sale_rate", tr("Sale"), "integer", true, 50);
            columns[20] = Column("tier_quantity1", tr("Tier Q1"), "integer", false, 50);
            columns[21] = Column("tier_price1", tr("Tier P1"), "double", false, 50);
            columns[22] = Column("tier_quantity2", tr("Tier Q2"), "integer", false, 50);
            columns[23] = Column("tier_price2", tr("Tier P2"), "double", false, 50);
            columns[24] = Column("tier_quantity3", tr("Tier Q3"), "integer", false, 50);
            columns[25] = Column("tier_price3", tr("Tier P3"), "double", false, 50);
            columns[26] = Column("my_weight", tr("My Weight"), "double", false, 50);
            sortColumn = 2;
            sortOrder = Qt::AscendingOrder;
            break;
        }
    }
    QString getSqlTableName() {
        return sqlTable;
    }
    int getNumberOfColumns() {
        return columns.size();
    }
    QString getSqlColumnName(int column) {
        return columns[column].property.sqlName;
    }
    QString getColumnHeader(int column) {
        return columns[column].property.name;
    }
    bool isColumnVisible(int column) {
        return columns[column].property.visible;
    }
    void setColumnVisible(int column, bool visible) {
        columns[column].property.visible = visible;
    }
    int getColumnWidth(int column) {
        return columns[column].property.columnWidth;
    }
    int getSortColumn() {
        return sortColumn;
    }
    Qt::SortOrder getSortOrder() {
        return sortOrder;
    }

private:
    QString sqlTable;
    int sortColumn = 0;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;
    QMap<int, Column> columns;
};

#endif // DATAMODELS_H
