#include "additemdialog.h"
#include "datamodel.h"
#include "listmodeldelegate.h"
#include "sqldatabase.h"
#include "ui_additemdialog.h"
#include "bricklink.h"
#include "config.h"

#include <QPixmap>

AddItemDialog::AddItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    // remove question mark from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

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
    connect(ui->priceInput, SIGNAL(valueChanged(QString)), this, SLOT(updateTotalCost()));
    connect(ui->quantityInput, SIGNAL(valueChanged(QString)), this, SLOT(updateTotalCost()));

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
        if(!Config::inst()->includeBrickArmsColors() && !Config::inst()->includeModulexColors()) {
            colorsModel->setFilter(QString("color_type NOT LIKE '%%1%' AND color_type NOT LIKE '%%2%'").arg("BrickArms").arg("Modulex"));
        } else if(!Config::inst()->includeBrickArmsColors()) {
            colorsModel->setFilter(QString("color_type NOT LIKE '%%1%'").arg("BrickArms"));
        } else if(!Config::inst()->includeModulexColors()) {
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
    m_category = new ItemType(cat);

    DataModel *categoriesDataModel = new DataModel(Tables::categories);
    DataModel *partsDataModel = new DataModel(m_category->tableName(), m_category->sqlName());

    // Set table source
    QSqlTableModel *categoriesModel = new QSqlTableModel(ui->categoriesListView, QSqlDatabase::database("catalogDatabase"));
    categoriesModel->setTable(categoriesDataModel->getSqlTableName());
    partsModel = new QSqlTableModel(ui->partsTableView, QSqlDatabase::database("catalogDatabase"));
    partsModel->setTable(partsDataModel->getSqlTableName());
    categoriesModel->setFilter(m_category->sqlName() + " == 1");

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
    QMap<QString, QVariant> fields;
    const QModelIndex categoryIndex = ui->categoriesListView->currentIndex();
    const QModelIndex colorIndex = ui->colorsListView->currentIndex();
    const QModelIndex partsIndex = ui->partsTableView->currentIndex();
    fields["Status"] = "I";
    fields["item_no"] = partsIndex.siblingAtColumn(2).data(Qt::DisplayRole).toString();
    fields["item_name"] = partsIndex.siblingAtColumn(3).data(Qt::DisplayRole).toString();
    fields["Condition"] = (ui->radioButtonNew->isChecked() ? "N" : "U");
    fields["color_name"] = colorIndex.data(Qt::DisplayRole).toString();
    fields["quantity"] = ui->quantityInput->value();
    fields["unit_price"] = ui->priceInput->value();
    fields["Remarks"] = SqlDatabase::getContainerLabel(fields["item_no"].toString()) + (ui->remarksInput->text() == "" ? "" : "<" + ui->remarksInput->text());
    fields["category_name"] = categoryIndex.data(Qt::DisplayRole).toString();
    fields["item_type"] = ui->w_item_types->currentText();
    fields["category_id"] = categoryIndex.siblingAtColumn(0).data(Qt::DisplayRole).toInt();
    fields["color_id"] = colorIndex.siblingAtColumn(0).data(Qt::DisplayRole).toString();
    fields["ItemTypeID"] = ui->w_item_types->currentData().toChar();
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
    getImage(selectedPart, m_category->apiName());
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

/**
 * @brief Slot function to calculate the total cost in the Add Item window
 *
 * This slot gets triggered when either the Quantity or the Price fiels
 * gets updated.
 */
void AddItemDialog::updateTotalCost() {
    ui->totalInput->setValue(ui->quantityInput->value() * ui->priceInput->value());
}





void AddItemDialog::getImage(QString part, QString itemType)
{
    nam = new QNetworkAccessManager(this);

    ui->w_picture->clear();
    ui->w_picture->setText("Loading image...");

    QUrl url("http:" + BrickLink::inst()->getItemInformation(itemType, part));
    qDebug() << "Loading image:" << BrickLink::inst()->getItemInformation(itemType, part);

    QNetworkReply* reply = nam->get(QNetworkRequest(url));

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    finishedSlot(reply);
}

void AddItemDialog::finishedSlot(QNetworkReply *reply)
{
    // Reading attributes of the reply
    // e.g. the HTTP status code
    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    // Or the target URL if it was a redirect:
    QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    // no error received?
    if (reply->error() == QNetworkReply::NoError) {
        // read data from QNetworkReply here
        QByteArray bytes = reply->readAll();  // bytes
        QPixmap pixmap;
        pixmap.loadFromData(bytes);
        int w = ui->w_picture->width();
        int h = ui->w_picture->height();
        ui->w_picture->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
    // Some http error received
    else {
        const QPixmap noImage(":/images/no_image.jpg");
        int w = ui->w_picture->width();
        int h = ui->w_picture->height();
        ui->w_picture->setPixmap(noImage.scaled(w,h,Qt::KeepAspectRatio));
    }

    // We receive ownership of the reply object
    // and therefore need to handle deletion.
    delete reply;
}
