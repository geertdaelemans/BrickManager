#include "labelsdialog.h"
#include "ui_labelsdialog.h"

#include "datamodel.h"
#include "sqldatabase.h"

#include <QtSql>
#include <QSqlQuery>
#include <QSqlQueryModel>

LabelsDialog::LabelsDialog(QWidget *parent, QString database) :
    QDialog(parent),
    ui(new Ui::LabelsDialog), m_database(database)
{
    // remove question mark from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->setupUi(this);

    // Create the data models
    DataModel *labelsDataModel = new DataModel(Tables::storage);
    int numberOfColumns = labelsDataModel->getNumberOfColumns();
    labelsModel = new QSqlTableModel(ui->tableView, QSqlDatabase::database("catalogDatabase"));
    labelsModel->setTable(labelsDataModel->getSqlTableName());
    for(int i = 0; i < numberOfColumns; i++) {
        labelsModel->setHeaderData(i, Qt::Horizontal, labelsDataModel->getColumnHeader(i));
    }

    // Set proxy model to enable sorting columns:
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(labelsModel);
    proxyModel->sort(labelsDataModel->getSortColumn(), labelsDataModel->getSortOrder());

    // Design the model and hide columns not needed:
    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    for(int i = 0; i < numberOfColumns; i++) {
        ui->tableView->setColumnHidden(i, !labelsDataModel->isColumnVisible(i));
        ui->tableView->setColumnWidth(i, labelsDataModel->getColumnWidth(i));
    }

    ui->tableView->setModel(proxyModel);

    if (m_database == nullptr)
        ui->updatePushButton->setDisabled(true);

    refreshLabelList();
}

LabelsDialog::~LabelsDialog()
{
    delete ui;
}

void LabelsDialog::refreshLabelList()
{
    if (labelsModel) {
        // Populate the model:
        if (!labelsModel->select()) {
            qDebug() << "labelsModel Error" << labelsModel->lastError();
            return;
        }
        while(labelsModel->canFetchMore())
            labelsModel->fetchMore();
    }
}

void LabelsDialog::on_updatePushButton_clicked()
{
    QList<Container> newLabels = SqlDatabase::getLabels(m_database);
    SqlDatabase::importLabels(newLabels);
    foreach (Container label, newLabels) {
        qDebug() << "New item" << label.getItemID() << "added to" << label.getName();
    }
    refreshLabelList();
}

void LabelsDialog::on_clearPushButton_clicked()
{
    SqlDatabase::clearAllLabels();
    refreshLabelList();
}
