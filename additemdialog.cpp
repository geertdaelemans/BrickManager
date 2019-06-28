#include "additemdialog.h"
#include "datamodel.h"
#include "listmodeldelegate.h"
#include "sqldatabase.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);

    // Populate Item Types
    ui->w_item_types_label->setText(tr("Item types:"));
    ui->w_item_types->addItem("Book", 'B');
    ui->w_item_types->addItem("Catalog", 'C');
    ui->w_item_types->addItem("Gear", 'G');
    ui->w_item_types->addItem("Instruction", 'I');
    ui->w_item_types->addItem("Minifig", 'M');
    ui->w_item_types->addItem("Original box", 'O');
    ui->w_item_types->addItem("Part", 'P');
    ui->w_item_types->addItem("Set", 'S');
    ui->w_item_types->setCurrentIndex(6);

    connect(ui->w_item_types, SIGNAL(currentTextChanged(QString)), this, SLOT(updateCategories(QString)));

    // Create the data models
    DataModel *colorsDataModel = new DataModel(Tables::colors);
    colorsModel = new QSqlTableModel(ui->colorsListView, QSqlDatabase::database("catalogDatabase"));
    colorsModel->setTable(colorsDataModel->getSqlTableName());

    updateCategories();

    // Set database relations
    int colorIdx = colorsModel->fieldIndex("color_name");
    colorsModel->setHeaderData(colorIdx, Qt::Horizontal, "Color");

    // Set proxy model to enable sorting columns
    QSortFilterProxyModel *colorsProxyModel = new QSortFilterProxyModel(this);
    colorsProxyModel->setSourceModel(colorsModel);
    colorsProxyModel->sort(colorsDataModel->getSortColumn(), colorsDataModel->getSortOrder());

    // Design the model
    ui->colorsListView->setModel(colorsProxyModel);
    ui->colorsListView->setModelColumn(colorIdx);

    // Apply delegates
    QItemDelegate *delegate = new ListModelDelegate(this);
    ui->colorsListView->setItemDelegate(delegate);
    ui->categoriesListView->setItemDelegate(delegate);

    colorsModel->select();
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::setColorFilter(QString itemName)
{
    bool reducedList = false;   // Reduced color list set to false by default

    // Reduce color list if possible
    if(itemName != "") {
        QList<int> list = SqlDatabase::getColorsOfPart(itemName);
        if (list.size()) {
            QString filterString;
            for(int i = 0; i < list.size(); i++) {
                filterString += "id=" + QString::number(list[i]);
                if(i < list.size()-1)
                    filterString += " OR " ;
            }
            colorsModel->setFilter(filterString);
            reducedList = true;
        }
    }

    // Set filter to include BrickArm and/or Modulex colors
    if (!reducedList) {
        QSettings settings;
        if(!settings.value("filter/includeBrickArmsColors").toBool() && !settings.value("filter/includeModulexColors").toBool()) {
            colorsModel->setFilter(QString("color_type NOT LIKE '%%1%' AND color_type NOT LIKE '%%2%'").arg("BrickArms").arg("Modulex"));
        } else if(!settings.value("filter/includeBrickArmsColors").toBool()) {
            colorsModel->setFilter(QString("color_type NOT LIKE '%%1%'").arg("BrickArms"));
        } else if(!settings.value("filter/includeModulexColors").toBool()) {
            colorsModel->setFilter(QString("color_type NOT LIKE '%%1%'").arg("Modulex"));
        }
    }

    m_colorSelected = false;

    if (m_lastSelectedColor != -1) {
        int numRows = ui->colorsListView->model()->rowCount();
        for (int i = 0; i < numRows; i++) {
            QModelIndex currentIndex = ui->colorsListView->model()->index(i, 0);
            int colorIndex = ui->colorsListView->model()->data(currentIndex).toInt();
            if (colorIndex == m_lastSelectedColor) {
                ui->colorsListView->selectionModel()->setCurrentIndex(currentIndex.sibling(currentIndex.row(), 1), QItemSelectionModel::Select);
                m_colorSelected = true;
            }
        }
    }

    statusAddButton();
}

void AddItemDialog::updateCategories(QString cat)
{
    QString category;
    Tables table = Tables::parts;
    if (cat == "Book") {
        category = "books";
        table = Tables::books;
    } else if (cat == "Catalog") {
        category = "catalogs";
        table = Tables::catalogs;
    } else if (cat == "Gear") {
        category = "gear";
        table = Tables::gear;
    } else if (cat == "Instruction") {
        category = "instructions";
        table = Tables::instructions;
    } else if (cat == "Minifig") {
        category = "minifigs";
        table = Tables::minifigs;
    } else if (cat == "Original box") {
        category = "boxes";
        table = Tables::boxes;
    } else if (cat == "Set") {
        category = "sets";
        table = Tables::sets;
    } else {
        category = "parts";
        table = Tables::parts;
    }

    DataModel *categoriesDataModel = new DataModel(Tables::categories);
    DataModel *partsDataModel = new DataModel(table);

    // Set table source
    QSqlTableModel *categoriesModel = new QSqlTableModel(ui->categoriesListView, QSqlDatabase::database("catalogDatabase"));
    categoriesModel->setTable(categoriesDataModel->getSqlTableName());
    partsModel = new QSqlTableModel(ui->partsTableView, QSqlDatabase::database("catalogDatabase"));
    partsModel->setTable(partsDataModel->getSqlTableName());
    categoriesModel->setFilter(category + " == 1");

    int categoryIdx = categoriesModel->fieldIndex("category_name");

    QSortFilterProxyModel *categoriesProxyModel = new QSortFilterProxyModel(this);
    categoriesProxyModel->setSourceModel(categoriesModel);
    categoriesProxyModel->sort(categoriesDataModel->getSortColumn(), categoriesDataModel->getSortOrder());
    QSortFilterProxyModel *partsProxyModel = new QSortFilterProxyModel(this);
    partsProxyModel->setSourceModel(partsModel);
    partsProxyModel->sort(partsDataModel->getSortColumn(), partsDataModel->getSortOrder());

    ui->categoriesListView->setModel(categoriesProxyModel);
    ui->categoriesListView->setModelColumn(categoryIdx);
    ui->partsTableView->setModel(partsProxyModel);

    for(int i = 0; i < 9; i++) {
        ui->partsTableView->setColumnHidden(i, !partsDataModel->isColumnVisible(i));
        partsModel->setHeaderData(i, Qt::Horizontal, partsDataModel->getColumnHeader(i));
        ui->partsTableView->setColumnWidth(i, partsDataModel->getColumnWidth(i));
    }

    categoriesModel->select();
    partsModel->select();

    m_categorySelected = false;
    statusAddButton();

    setColorFilter();
}

void AddItemDialog::on_addPushButton_clicked()
{
    QList<QString> fields;
    const QModelIndex categoryIndex = ui->categoriesListView->currentIndex();
    const QModelIndex colorName = ui->colorsListView->currentIndex();
    const QModelIndex colorIndex = colorName.siblingAtColumn(0);
    const QModelIndex partsIndex = ui->partsTableView->currentIndex();
    const QModelIndex partsName = partsIndex.siblingAtColumn(3);
    const QModelIndex partsNumber = partsIndex.siblingAtColumn(2);
    const QString itemTypeName = ui->w_item_types->currentText();
    const QVariant itemTypeID = ui->w_item_types->currentData();
    fields.append(categoryIndex.data(Qt::DisplayRole).toString());
    fields.append(colorIndex.data(Qt::DisplayRole).toString());
    fields.append(colorName.data(Qt::DisplayRole).toString());
    fields.append(partsName.data(Qt::DisplayRole).toString());
    fields.append(partsNumber.data(Qt::DisplayRole).toString());
    fields.append(itemTypeName);
    fields.append(itemTypeID.toChar());
    emit insertItem(fields);
}

void AddItemDialog::on_categoriesListView_clicked(const QModelIndex &index)
{
    const QModelIndex categoryID = index.sibling(index.row(), 0);
    int selectedCategory = categoryID.data(Qt::DisplayRole).toInt();
    partsModel->setFilter(QString("category_id == %1").arg(selectedCategory));
    partsModel->select();
    m_categorySelected = true;
    m_partSelected = false;
    statusAddButton();
    setColorFilter();
}


void AddItemDialog::on_partsTableView_clicked(const QModelIndex &index)
{
    // Retrieve the part number from column 2
    const QModelIndex partNumber = index.sibling(index.row(), 2);
    QString selectedPart = partNumber.data(Qt::DisplayRole).toString();

    setColorFilter(selectedPart);

    m_partSelected = true;
    statusAddButton();
}

void AddItemDialog::on_colorsListView_clicked(const QModelIndex &index)
{
    // Get color ID in first column
    m_lastSelectedColor = index.siblingAtColumn(0).data(Qt::DisplayRole).toInt();
    m_colorSelected = true;
    statusAddButton();
}

void AddItemDialog::statusAddButton() {
    // When category, part and color is selected, enable Add button
    ui->addPushButton->setEnabled(m_categorySelected && m_partSelected && m_colorSelected);
};
