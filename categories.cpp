#include "categories.h"
#include "ui_categories.h"

#include <QtWidgets>

Categories::Categories(QWidget *parent, const QString &table) :
    QDialog(parent),
    ui(new Ui::Categories)
{
    ui->setupUi(this);

    // Create the data model:
    model = new QSqlRelationalTableModel(ui->tableView);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(table);

    // Remember the indexes of the columns:
    int idIdx = model->fieldIndex("category_id");
    int nameIdx = model->fieldIndex("category_name");
    int parentIdx = model->fieldIndex("parent_id");

    // Set the localized header captions:
    model->setHeaderData(idIdx, Qt::Horizontal, tr("ID"));
    model->setHeaderData(nameIdx, Qt::Horizontal, tr("Naam"));
    model->setHeaderData(parentIdx, Qt::Horizontal, tr("Parent"));

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    // Set the model and hide the ID column:
    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    // Populate the model:
    if (!model->select()) {
        showError(model->lastError());
        return;
    }


}

void Categories::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

Categories::~Categories()
{
    delete ui;
}
