#include "listmodel.h"
#include "ui_categories.h"
#include "bricklink.h"
#include "listmodeldelegate.h"

#include <QtWidgets>

ListModel::ListModel(QWidget *parent, Tables table, int orderID) :
    QDialog(parent),
    ui(new Ui::Categories)
{
    ui->setupUi(this);
    p_tableModel = new TableModel(table, orderID);
    int numberOfColumns = p_tableModel->getNumberOfColumns();

    // Create the data model:
    model = new QSqlRelationalTableModel(ui->tableView);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(p_tableModel->getSqlTableName());
    for(int i = 0; i < numberOfColumns; i++) {
        model->setHeaderData(i, Qt::Horizontal, p_tableModel->getColumnHeader(i));
    }

    // Set database relations
    int colorIdx = model->fieldIndex("color_id");
    model->setRelation(colorIdx, QSqlRelation("colors", "color_id", "color_name"));
    int categoryIdx = model->fieldIndex("category_id");
    model->setRelation(categoryIdx, QSqlRelation("categories", "category_id", "category_name"));

    // Set proxy model to enable sorting columns:
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->sort(p_tableModel->getSortColumn(), p_tableModel->getSortOrder());

    // Design the model and hide columns not needed:
    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    for(int i = 0; i < numberOfColumns; i++) {
        ui->tableView->setColumnHidden(i, !p_tableModel->isColumnVisible(i));
        ui->tableView->setColumnWidth(i, p_tableModel->getColumnWidth(i));
    }

    // Apply delegates
    QItemDelegate *delegate = new ListModelDelegate(this);
    ui->tableView->setItemDelegateForColumn(colorIdx, delegate);
    int dateIdx = model->fieldIndex("date_created");
    ui->tableView->setItemDelegateForColumn(dateIdx, delegate);

    // Connect SLOT to context menu
    connect(ui->tableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotCustomMenuRequested(QPoint)));

    // Populate the model:
    if (!model->select()) {
        showError(model->lastError());
        return;
    }
    while(model->canFetchMore())
        model->fetchMore();
}


ListModel::~ListModel()
{
     delete ui;
}


void ListModel::slotCustomMenuRequested(const QPoint pos) {
    QMenu *p_popUpMenu = new QMenu(this);

     // Configuration tab including order and visibility
     p_popUpMenu->addAction(new QAction("Configure", this));
     p_popUpMenu->addSeparator();

    // List all possible fields and select visibility
    for (int i = 0; i < p_tableModel->getNumberOfColumns(); i++) {
        QCheckBox *p_checkBox = new QCheckBox(p_popUpMenu);
        p_checkBox->setText(p_tableModel->getColumnHeader(i));
        p_checkBox->setObjectName(QString::number(i));
        p_checkBox->setChecked(p_tableModel->isColumnVisible(i));
        QWidgetAction *p_checkableAction = new QWidgetAction(p_popUpMenu);
        p_checkableAction->setDefaultWidget(p_checkBox);
        p_popUpMenu->addAction(p_checkableAction);
        connect(p_checkBox, &QCheckBox::toggled, this, &ListModel::setVisibilityFromCheckBox);
    }
    QPoint *p_position = new QPoint(pos.x(), pos.y());
    p_position->ry() -= 30; // Correct off set popup window
    p_popUpMenu->popup(ui->tableView->viewport()->mapToGlobal(*p_position));
};


void ListModel::setVisibilityFromCheckBox() {
    auto *p_checkBox = qobject_cast<QCheckBox *>(sender());
    int index = p_checkBox->objectName().toInt();
    p_tableModel->setColumnVisible(index, p_checkBox->isChecked());
    ui->tableView->setColumnHidden(index, !p_tableModel->isColumnVisible(index));
}

void ListModel::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}
