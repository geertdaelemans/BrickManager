#include "categories.h"
#include "ui_categories.h"

#include <QtWidgets>

Categories::Categories(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Categories)
{
    ui->setupUi(this);

    // Create the data model:
    model = new QSqlRelationalTableModel(ui->tableView);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("categories");

    // Remember the indexes of the columns:
    int idIdx = model->fieldIndex("category_id");
    int nameIdx = model->fieldIndex("category_name");

    // Set the localized header captions:
    model->setHeaderData(idIdx, Qt::Horizontal, tr("ID"));
    model->setHeaderData(nameIdx, Qt::Horizontal, tr("Naam"));

    // Populate the model:
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    // Set the model and hide the ID column:
    ui->tableView->setModel(model);
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
