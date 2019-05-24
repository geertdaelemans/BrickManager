#include "additemdialog.h"
#include "datamodel.h"
#include "listmodeldelegate.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);

    // Create the data models
    DataModel *colorsDataModel = new DataModel(Tables::colors);
    DataModel *categoriesDataModel = new DataModel(Tables::categories);
    DataModel *partsDataModel = new DataModel(Tables::parts);

    QSqlTableModel *colorsModel = new QSqlTableModel(ui->colorsListView);
    colorsModel->setTable(colorsDataModel->getSqlTableName());

    // Set filter to include BrickArm and/or Modulex colors
    QSettings settings;
    if(!settings.value("filter/includeBrickArmsColors").toBool() && !settings.value("filter/includeModulexColors").toBool()) {
        colorsModel->setFilter(QString("color_type NOT LIKE '%%1%' AND color_type NOT LIKE '%%2%'").arg("BrickArms").arg("Modulex"));
    } else if(!settings.value("filter/includeBrickArmsColors").toBool()) {
        colorsModel->setFilter(QString("color_type NOT LIKE '%%1%'").arg("BrickArms"));
    } else if(!settings.value("filter/includeModulexColors").toBool()) {
        colorsModel->setFilter(QString("color_type NOT LIKE '%%1%'").arg("Modulex"));
    }

    // Set table source
    QSqlTableModel *categoriesModel = new QSqlTableModel(ui->categoriesListView);
    categoriesModel->setTable(categoriesDataModel->getSqlTableName());
    partsModel = new QSqlTableModel(ui->partsTableView);
    partsModel->setTable(partsDataModel->getSqlTableName());
    categoriesModel->setFilter("part == 1");


    // Set database relations
    int colorIdx = colorsModel->fieldIndex("color_name");
    colorsModel->setHeaderData(colorIdx, Qt::Horizontal, "Color");
    int categoryIdx = categoriesModel->fieldIndex("category_name");

    // Set proxy model to enable sorting columns
    QSortFilterProxyModel *colorsProxyModel = new QSortFilterProxyModel(this);
    colorsProxyModel->setSourceModel(colorsModel);
    colorsProxyModel->sort(colorsDataModel->getSortColumn(), colorsDataModel->getSortOrder());
    QSortFilterProxyModel *categoriesProxyModel = new QSortFilterProxyModel(this);
    categoriesProxyModel->setSourceModel(categoriesModel);
    categoriesProxyModel->sort(categoriesDataModel->getSortColumn(), categoriesDataModel->getSortOrder());
    QSortFilterProxyModel *partsProxyModel = new QSortFilterProxyModel(this);
    partsProxyModel->setSourceModel(partsModel);
    partsProxyModel->sort(partsDataModel->getSortColumn(), partsDataModel->getSortOrder());

    // Design the model
    ui->colorsListView->setModel(colorsProxyModel);
    ui->colorsListView->setModelColumn(colorIdx);
    ui->categoriesListView->setModel(categoriesProxyModel);
    ui->categoriesListView->setModelColumn(categoryIdx);
    ui->partsTableView->setModel(partsProxyModel);

    for(int i = 0; i < 9; i++) {
        ui->partsTableView->setColumnHidden(i, !partsDataModel->isColumnVisible(i));
        partsModel->setHeaderData(i, Qt::Horizontal, partsDataModel->getColumnHeader(i));
        ui->partsTableView->setColumnWidth(i, partsDataModel->getColumnWidth(i));
    }

    // Apply delegates
    QItemDelegate *delegate = new ListModelDelegate(this);
    ui->colorsListView->setItemDelegate(delegate);
    ui->categoriesListView->setItemDelegate(delegate);

    colorsModel->select();
    categoriesModel->select();
    partsModel->select();
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::on_addPushButton_clicked()
{
    QList<QString> fields;
    const QModelIndex categoryIndex = ui->categoriesListView->currentIndex();
    const QModelIndex colorIndex = ui->colorsListView->currentIndex();
    const QModelIndex partsIndex = ui->partsTableView->currentIndex();
    const QModelIndex partsName = partsIndex.sibling(partsIndex.row(), 3);
    const QModelIndex partsNumber = partsIndex.sibling(partsIndex.row(), 2);
    fields.append(categoryIndex.data(Qt::DisplayRole).toString());
    fields.append(colorIndex.data(Qt::DisplayRole).toString());
    fields.append(partsName.data(Qt::DisplayRole).toString());
    fields.append(partsNumber.data(Qt::DisplayRole).toString());
    emit insertItem(fields);
}

void AddItemDialog::on_categoriesListView_clicked(const QModelIndex &index)
{
    const QModelIndex categoryID = index.sibling(index.row(), 0);
    int selectedCategory = categoryID.data(Qt::DisplayRole).toInt();
    partsModel->setFilter(QString("category_id == %1").arg(selectedCategory));
    partsModel->select();
}
