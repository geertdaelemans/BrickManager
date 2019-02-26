#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordersdialog.h"
#include "settingsdialog.h"
#include "additemdialog.h"
#include "sqldatabase.h"
#include "listmodel.h"
#include "datamodel.h"
#include "simplepopup.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QtXml>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();

    SqlDatabase mydB = SqlDatabase();

    bricklink.importColors();
    bricklink.importCategories();

    Q_UNUSED(mydB)
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Adds tab to the tabWidget, left most position. If tab already
 * exists, activate that tab.
 *
 * @param page Pointer to widget to be displayed.
 * @param label Pointer to label string of tab.
 * @return tab id of the added tab.
 */
int MainWindow::addTab(QWidget *page, const QString &label)
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
        tabs.append(label);
    }
    // Activate current tab
    ui->tabWidget->setCurrentIndex(tabNumber);
    return tabNumber;
}

/**
 * Removes tab from tabWidget.
 *
 * @param index Index of tab to be removed.
 * @return nothing.
 */
void MainWindow::removeTab(int index)
{
    tabs.removeAll(ui->tabWidget->tabText(index));
    ui->tabWidget->removeTab(index);
}


/**
 * @brief MainWindow File Menu
 */


void MainWindow::on_actionNew_triggered()
{
    // Prepare data model
    QString sqlTableName = "Untitled";
    DataModel *p_dataModel = new DataModel(Tables::brickstock, sqlTableName);
    p_dataModel->initiateSqlTableAuto();

    // Prepare list view
    listModel = new ListModel(this, p_dataModel);

    // Add tab
    addTab(listModel, sqlTableName);
}


void MainWindow::on_actionOrders_triggered()
{
    ordersDialog = new OrdersDialog(this);
    ordersDialog->setModal(true);
    ordersDialog->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    QObject::connect(ordersDialog, SIGNAL(ordersSelected(QList<QString>)), this, SLOT(openInventoryTab(QList<QString>)));

    ordersDialog->exec();
}


void MainWindow::on_actionMy_Inventory_triggered()
{
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

    // Prepare list
    DataModel *p_dataModel = new DataModel(Tables::userinventories);
    listModel = new ListModel(this, p_dataModel);
    QString header = "My Inventory";
    addTab(listModel, header);
}


void MainWindow::openInventoryTab(QList<QString> orderIDs)
{
    int numberOfTabs = ui->tabWidget->count();
    for(int i = 0; i < numberOfTabs; i++)
    {
        tabs.append(ui->tabWidget->tabText(i));
    }
    foreach(QString orderID, orderIDs)
    {
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
        ListModel *inv = new ListModel(this, p_dataModel);
        addTab(inv, "Order#" + orderID);
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
        QString sqlTableName = tableName;
        sqlTableName.remove(QRegExp("[^a-zA-Z\\d]"));

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
        DataModel *p_dataModel = new DataModel(Tables::brickstock, sqlTableName);
        p_dataModel->initiateSqlTableAuto();

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
        listModel = new ListModel(this, p_dataModel);
        addTab(listModel, tableName);
    }
}


void MainWindow::on_actionColors_triggered()
{
    DataModel *p_dataModel = new DataModel(Tables::colors);
    listModel = new ListModel(this, p_dataModel);
    listModel->exec();
}


void MainWindow::on_actionCategories_triggered()
{
    DataModel *p_dataModel = new DataModel(Tables::categories);
    listModel = new ListModel(this, p_dataModel);
    listModel->exec();
}


void MainWindow::on_actionSettings_triggered()
{
    settingsDialog = new SettingsDialog(this);
    settingsDialog->exec();
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
    AddItemDialog *addItemDialog = new AddItemDialog();
    addItemDialog->exec();
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
