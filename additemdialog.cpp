#include "additemdialog.h"
#include "datamodel.h"
#include "listmodeldelegate.h"
#include "ui_additemdialog.h"

#include <QtSql>

AddItemDialog::AddItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);

    // Create the data models
    DataModel *colorsDataModel = new DataModel(Tables::colors);
    DataModel *categoriesDataModel = new DataModel(Tables::categories);

    QSqlRelationalTableModel *colorsModel = new QSqlRelationalTableModel(ui->colorsListView);
    colorsModel->setTable(colorsDataModel->getSqlTableName());
    QSqlRelationalTableModel *categoriesModel = new QSqlRelationalTableModel(ui->categoriesListView);
    categoriesModel->setTable(categoriesDataModel->getSqlTableName());

    // Set database relations
    int colorIdx = colorsModel->fieldIndex("color_id");
    colorsModel->setRelation(colorIdx, QSqlRelation("colors", "color_id", "color_name"));
    colorsModel->setHeaderData(colorIdx, Qt::Horizontal, "Color");
    int categoryIdx = categoriesModel->fieldIndex("category_id");
    categoriesModel->setRelation(categoryIdx, QSqlRelation("categories", "category_id", "category_name"));

    // Set proxy model to enable sorting columns
    QSortFilterProxyModel *colorsProxyModel = new QSortFilterProxyModel(this);
    colorsProxyModel->setSourceModel(colorsModel);
    colorsProxyModel->sort(colorsDataModel->getSortColumn(), colorsDataModel->getSortOrder());
    QSortFilterProxyModel *categoriesProxyModel = new QSortFilterProxyModel(this);
    categoriesProxyModel->setSourceModel(categoriesModel);
    categoriesProxyModel->sort(categoriesDataModel->getSortColumn(), categoriesDataModel->getSortOrder());

    // Design the model
    ui->colorsListView->setModel(colorsProxyModel);
    ui->colorsListView->setModelColumn(colorIdx);
    ui->categoriesListView->setModel(categoriesProxyModel);
    ui->categoriesListView->setModelColumn(categoryIdx);

    // Apply delegates
    QItemDelegate *delegate = new ListModelDelegate(this);
    ui->colorsListView->setItemDelegate(delegate);
    ui->categoriesListView->setItemDelegate(delegate);

    colorsModel->select();
    categoriesModel->select();
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}
