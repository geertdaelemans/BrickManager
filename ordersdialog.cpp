#include "ordersdialog.h"
#include "ui_ordersdialog.h"
#include "bricklink.h"
#include "simplepopup.h"
#include "datamodel.h"
#include "listmodeldelegate.h"

#include <QMenu>
#include <QWidgetAction>
#include <QMessageBox>
#include <QItemDelegate>

OrdersDialog::OrdersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrdersDialog)
{
    // Set-up or reset Orders SQL table
    p_dataModel = new DataModel(Tables::orders);
    p_dataModel->initiateSqlTable("tempDatabase");

    // Import Order Inventory with unfiled order by default
    bricklink.importOrders(false);

    // Show Downloading... message
    SimplePopup *p_popup = new SimplePopup(this);
    p_popup->move(this->width()/2-p_popup->width()/2, this->height()/2-p_popup->height()/2);
    p_popup->show();

    // Wait for confirmation that data has been loaded in SQL database
    QEventLoop loop;
    connect(&bricklink, SIGNAL(dataBaseUpdatedWithOrders()), &loop, SLOT(quit()));
    loop.exec();

    // Hide message box
    p_popup->hide();

    // Prepare List
    ui->setupUi(this);

    // Create the data model:
    model = new GenericTableModel(ui->view, QSqlDatabase::database("tempDatabase"));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(p_dataModel->getSqlTableName());
    int numberOfColumns = p_dataModel->getNumberOfColumns();
    for(int i = 0; i < numberOfColumns; i++) {
        model->setHeaderData(i, Qt::Horizontal, p_dataModel->getColumnHeader(i));
    }

    // Set proxy model to enable sorting columns:
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->sort(p_dataModel->getSortColumn(), p_dataModel->getSortOrder());

    // Set delegates
    QAbstractItemDelegate *delegate = new ListModelDelegate(this);
    ui->view->setItemDelegate(delegate);

    // Design the model and hide columns not needed:
    ui->view->setModel(proxyModel);
    ui->view->horizontalHeader()->setSectionsMovable(true);
    ui->view->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    for(int i = 0; i < numberOfColumns; i++) {
        ui->view->setColumnHidden(i, !p_dataModel->isColumnVisible(i));
        ui->view->setColumnWidth(i, p_dataModel->getColumnWidth(i));
    }

    // Connect SLOT to context menu
    connect(ui->view->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(slotCustomMenuRequested(QPoint)));

    // Populate the model:
    if (!model->select()) {
        showError(model->lastError());
        return;
    }
}


OrdersDialog::~OrdersDialog()
{
    p_dataModel->truncateSqlTable();
    delete ui;
}

void OrdersDialog::reject()
{
    p_dataModel->truncateSqlTable();
    QDialog::reject();
}

void OrdersDialog::slotCustomMenuRequested(const QPoint pos) {
    QMenu *p_popUpMenu = new QMenu(this);

     // Configuration tab including order and visibility
     p_popUpMenu->addAction(new QAction("Configure", this));
     p_popUpMenu->addSeparator();

    // List all possible fields and select visibility
    for (int i = 0; i < p_dataModel->getNumberOfColumns(); i++) {
        QCheckBox *p_checkBox = new QCheckBox(p_popUpMenu);
        p_checkBox->setText(p_dataModel->getColumnHeader(i));
        p_checkBox->setObjectName(QString::number(i));
        p_checkBox->setChecked(p_dataModel->isColumnVisible(i));
        QWidgetAction *p_checkableAction = new QWidgetAction(p_popUpMenu);
        p_checkableAction->setDefaultWidget(p_checkBox);
        p_popUpMenu->addAction(p_checkableAction);
        connect(p_checkBox, &QCheckBox::toggled, this, &OrdersDialog::setVisibilityFromCheckBox);
    }
    QPoint *p_position = new QPoint(pos.x(), pos.y());
    p_position->ry() -= 30; // Correct off set popup window
    p_popUpMenu->popup(ui->view->viewport()->mapToGlobal(*p_position));
};


void OrdersDialog::setVisibilityFromCheckBox() {
    auto *p_checkBox = qobject_cast<QCheckBox *>(sender());
    int index = p_checkBox->objectName().toInt();
    p_dataModel->setColumnVisible(index, p_checkBox->isChecked());
    ui->view->setColumnHidden(index, !p_dataModel->isColumnVisible(index));
}


void OrdersDialog::on_checkBoxFiled_stateChanged(int filed)
{
    // Reset SQL table
    p_dataModel->truncateSqlTable();
    p_dataModel->initiateSqlTable();

    // Import Order Inventory with (un)filed parameter
    bricklink.importOrders(filed);

    // Show Downloading... message
    SimplePopup *p_popup = new SimplePopup(this);
    p_popup->move(this->width()/2-p_popup->width()/2, this->height()/2-p_popup->height()/2);
    p_popup->show();

    // Wait for confirmation that data has been loaded in SQL database
    QEventLoop loop;
    connect(&bricklink, SIGNAL(dataBaseUpdatedWithOrders()), &loop, SLOT(quit()));
    loop.exec();

    // Hide message box
    p_popup->hide();

    // Prepare List
    if (!model->select()) {
        showError(model->lastError());
        return;
    }
}

void OrdersDialog::on_pushButton_clicked()
{
    QItemSelectionModel *select = ui->view->selectionModel();
    QModelIndexList rows = select->selectedRows(); // return selected row(s)
    QList<QString> list;
    foreach(QModelIndex index, rows) {
        list.append(ui->view->model()->data(ui->view->model()->index(index.row(), 0)).toString());
    }
    emit ordersSelected(list);
}


void OrdersDialog::on_view_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    OrdersDialog::on_pushButton_clicked();
}


void OrdersDialog::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}
