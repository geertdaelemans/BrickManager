#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordersdialog.h"
#include "settingsdialog.h"
#include "additemdialog.h"
#include "sqldatabase.h"
#include "listmodel.h"
#include "datamodel.h"
#include "simplepopup.h"
#include "exportxml.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QtXml>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();

    m_trans = new CTransfer();
    connect(m_trans, SIGNAL(updateStatusBar(QString, int)), this, SLOT(updateStatusBar(QString, int)));
    m_trans->init();

    SqlDatabase mydB = SqlDatabase();

    m_trans->brickLinkLogin();

    Q_UNUSED(mydB)
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief adds a tab to the main window
 *
 * Adds tab to the tabWidget, left most position. If tab already
 * exists, activate that tab.
 *
 * @param page Pointer to widget to be displayed.
 * @param label Pointer to label string of tab.
 * @return tab id of the added tab.
 */
int MainWindow::addTab(ListModel *page, const QString &label)
{
    int numberOfTabs = ui->tabWidget->count();
    int tabNumber = -1;     // -1 means nothing found
    // Search for tab with same name
    for(int i = 0; i < numberOfTabs; i++) {
        if (ui->tabWidget->tabText(i) == label) {
            tabNumber = i;
            break;
        }
    }
    // If no tab found, add new tab in leftmost position.
    if(tabNumber == -1) {
        tabNumber = ui->tabWidget->insertTab(0, page, label);
        tabList[label] = page;
    }
    // Activate current tab
    ui->tabWidget->setCurrentIndex(tabNumber);

    // Enable the Add Items menu
    if (!ui->actionAdd_Items->isEnabled())
        ui->actionAdd_Items->setEnabled(true);

    // Enable the Save As... dialog
    if (!ui->actionSave_As->isEnabled())
        ui->actionSave_As->setEnabled(true);

    // Enable the Close button
    if (!ui->actionClose->isEnabled())
        ui->actionClose->setEnabled(true);

    return tabNumber;
}


/**
 * @brief Display messages in statusbar
 *
 * Hides the normal status indications and displays the given message for the
 * specified number of milli-seconds (timeout). If timeout is 0 (default), the
 * message remains displayed until the next message.
 *
 * @param msg - Message to be displayed
 * @param timeout - Time in ms, 0 for continuous
 */
void MainWindow::updateStatusBar(QString msg, int timeout) {
    ui->statusbar->showMessage(msg, timeout);
}


/**
 * @brief Remove tab from tabWidget.
 *
 * @param index Index of tab to be removed.
 * @return nothing.
 */
void MainWindow::removeTab(int index)
{
    QString tabName = ui->tabWidget->tabText(index);    
    SqlDatabase::removeTable(tabName);
    tabList.remove(tabName);
    ui->tabWidget->removeTab(index);
    if (ui->tabWidget->count() == 0) {
        ui->actionSave_As->setDisabled(true);
        ui->actionClose->setDisabled(true);
        ui->actionAdd_Items->setDisabled(true);
    }
}


/**
 * @brief MainWindow File Menu
 */


void MainWindow::on_actionNew_triggered()
{
    // Prepare data model
    QString name = "Untitled";
    QString sqlTableName = SqlDatabase::getUniqueTableName(&name);
    DataModel *p_dataModel = new DataModel(Tables::brickstock, sqlTableName);
    p_dataModel->initiateSqlTableAuto("tempDatabase");

    // Prepare list view
    ListModel *listModel = new ListModel(this, p_dataModel, QSqlDatabase::database("tempDatabase"));

    // Add tab
    addTab(listModel, name);
}


void MainWindow::on_actionOrders_triggered()
{
    ordersDialog = new OrdersDialog(this);
    ordersDialog->setModal(true);
    ordersDialog->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    QObject::connect(ordersDialog, SIGNAL(ordersSelected(QList<QString>)), this, SLOT(openInventoryTab(QList<QString>)));

    ordersDialog->exec();
}


void MainWindow::on_actionSave_As_triggered()
{
    QString tabName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString sqlTableName = SqlDatabase::getTableName(tabName);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File as"), QString("d:\\%1").arg(tabName), tr("BrickStock XML Data (*.bsx)"));
    if (fileName != "") {
        ExportXml::SaveXMLFile(sqlTableName, fileName);
        QString name = QFileInfo(fileName).baseName().toLatin1();
        SqlDatabase::updateTableName(tabName, name);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), name);
    }
}


void MainWindow::on_actionMy_Inventory_triggered()
{
    // Prepare list
    QString header = "My Inventory";
    SqlDatabase::getUniqueTableName(&header, "userinventories");
    DataModel *p_dataModel = new DataModel(Tables::userinventories);
    p_dataModel->initiateSqlTableAuto("tempDatabase");

    // Import User Inventory through the BrickLink class
    bricklink.importUserInventory();

    // Show Downloading... message
    SimplePopup *p_popup = new SimplePopup(this);
    p_popup->move(this->width()/2-p_popup->width()/2, this->height()/2-p_popup->height()/2);
    p_popup->show();

    // Wait for confirmation that data has been loaded in SQL database
    QEventLoop loop;
    connect(&bricklink, SIGNAL(dataBaseUpdated()), &loop, SLOT(quit()));
    loop.exec();

    // Hide message box
    p_popup->hide();

    ListModel *listModel = new ListModel(this, p_dataModel, QSqlDatabase::database("tempDatabase"));
    addTab(listModel, header);
}

void MainWindow::insertItemIntoSheet(QList<QString> fields)
{
    ListModel *model = tabList[ui->tabWidget->tabText(ui->tabWidget->currentIndex())];
    model->insertRow(fields);
}

void MainWindow::openInventoryTab(QList<QString> orderIDs)
{
    foreach(QString orderID, orderIDs) {
        // Import User Inventory through the BrickLink class
        bricklink.importOrderItem(orderID.toInt());

        // Show Downloading... message
        SimplePopup *p_popup = new SimplePopup(this);
        p_popup->move(this->width()/2-p_popup->width()/2, this->height()/2-p_popup->height()/2);
        p_popup->show();

        // Wait for confirmation that data has been loaded in SQL database
        QEventLoop loop;
        connect(&bricklink, SIGNAL(dataBaseUpdated()), &loop, SLOT(quit()));
        //connect(&bricklink, &BrickLink::dataBaseUpdated, &loop, &QEventLoop::quit);
        loop.exec();

        // Hide message box
        p_popup->hide();

        // Prepare list
        DataModel *p_dataModel = new DataModel(Tables::orderitem, orderID);
        ListModel *inv = new ListModel(this, p_dataModel, QSqlDatabase::database("tempDatabase"));
        QString tabName = "Order#" + orderID;
        SqlDatabase::getUniqueTableName(&tabName, p_dataModel->getSqlTableName());
        tabList[tabName] = inv;
        addTab(inv, tabName);
    }
    ordersDialog->close();
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "d:\\", tr("Inventory Files (*.bsx)"));
    QFile file(fileName);
    if(!fileName.isEmpty() && !fileName.isNull()) {
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "error", file.errorString());
            return;
        }

        // Get file information
        QFileInfo info(file);
        QString tableName = info.baseName();

        //The QDomDocument class represents an XML document.
        QDomDocument xmlInventory;

        // Set data into the QDomDocument before processing
        xmlInventory.setContent(&file);
        file.close();

        // Extract the root markup
        QDomElement root = xmlInventory.documentElement();              // BrickStockXML
        QDomElement inventory = root.firstChild().toElement();          // Inventory
        QDomElement item = inventory.firstChild().toElement();          // Item

        // Prepare data model
        QString sqlTableName = SqlDatabase::getUniqueTableName(&tableName);
        DataModel *p_dataModel = new DataModel(Tables::brickstock, sqlTableName);
        p_dataModel->initiateSqlTableAuto("tempDatabase");

        // Read each child of the Inventory node
        while (!item.isNull()) {

            // Prepare fields
            QMap<QString, QVariant> fields;
            if (item.tagName() == "Item") {
                QDomElement field = item.firstChild().toElement();
                // Read Name and value
                while (!field.isNull()) {
                    fields[field.tagName()] = field.firstChild().toText().data();
                    if (fields[field.tagName()].isNull()) {
                        fields[field.tagName()] = QVariant(1);
                    }
                    // Next field
                    field = field.nextSibling().toElement();
                }
            }

            // Set original quantity to current quantity, if not already set
            if (!fields["OrigQty"].isValid())
                fields["OrigQty"] = fields["Qty"];

            // Set original unit price to current unit price, if not already set
            if (!fields["OrigPrice"].isValid())
                fields["OrigPrice"] = fields["Price"];

            // Add collected fields to SQL database
            QSqlError error = p_dataModel->addItemToTable(fields);

            // Next sibling
            item = item.nextSibling().toElement();
        }
        ListModel *listModel = new ListModel(this, p_dataModel, QSqlDatabase::database("tempDatabase"));
        addTab(listModel, tableName);
    }
}


void MainWindow::on_actionClose_triggered()
{
    removeTab(ui->tabWidget->currentIndex());
}


void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}


/**
 * @brief MainWindow Edit Menu
 */

void MainWindow::on_actionAdd_Items_triggered()
{
    AddItemDialog *addItemDialog = new AddItemDialog(this);
    QObject::connect(addItemDialog, SIGNAL(insertItem(QList<QString>)), this, SLOT(insertItemIntoSheet(QList<QString>)));
    addItemDialog->exec();
}


/**
 * @brief MainWindow Extra's Menu
 */

void MainWindow::on_actionSettings_triggered()
{
    settingsDialog = new SettingsDialog(this);
    settingsDialog->exec();
}

void MainWindow::on_actionUpdate_Database_triggered()
{
    ProgressDialog *pd = new ProgressDialog(this);
    m_trans->importCatalog(pd);
    pd->exec();
}

/**
 * @brief MainWindow Help Menu
 */

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Brick Manager"),
            tr("<p>The <b>Brick Manager</b> application is being developed by Geert Daelemans.</p>"));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    removeTab(index);
}


