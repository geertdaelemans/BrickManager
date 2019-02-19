#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordersdialog.h"
#include "settingsdialog.h"
#include "sqldatabase.h"
#include "listmodel.h"
#include "datamodels.h"
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
    TableModel *p_tableModel = new TableModel(Tables::userinventories);
    listModel = new ListModel(this, p_tableModel);
    QString header = "My Inventory";
    int numberOfTabs = ui->tabWidget->count();
    if(tabs.indexOf(header) == -1)
    {
        ui->tabWidget->addTab(listModel, header);
        ui->tabWidget->setCurrentIndex(numberOfTabs);
        tabs.append(header);
    } else {
        ui->tabWidget->setCurrentIndex(tabs.indexOf(header));
    }
}

void MainWindow::on_actionColors_triggered()
{
    TableModel *p_tableModel = new TableModel(Tables::colors);
    listModel = new ListModel(this, p_tableModel);
    listModel->exec();
}

void MainWindow::on_actionCategories_triggered()
{
    TableModel *p_tableModel = new TableModel(Tables::categories);
    listModel = new ListModel(this, p_tableModel);
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
    ui->tabWidget->removeTab(index);
    tabs.removeAt(index);
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
        int numberOfTabs = ui->tabWidget->count();
        if(tabs.indexOf("Order#" + orderID) == -1)
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
            TableModel *p_tableModel = new TableModel(Tables::orderitem, orderID);
            ListModel *inv = new ListModel(this, p_tableModel);
            ui->tabWidget->addTab(inv, "Order#" + orderID);
            ui->tabWidget->setCurrentIndex(numberOfTabs);
        }
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
        TableModel *p_tableModel = new TableModel(Tables::brickstock, sqlTableName);
        p_tableModel->initiateSqlTable();

        // Read each child of the Inventory node
        while (!item.isNull()) {

            // Prepare fields
            QMap<QString, QVariant> fields;
            if (item.tagName() == "Item") {
                QDomElement field = item.firstChild().toElement();
                // Read Name and value
                while (!field.isNull()) {
                    if (field.tagName() == "LotID")
                        fields["inventory_id"] = field.firstChild().toText().data().toInt();
                    if (field.tagName() == "ItemID")
                        fields["item_no"] = field.firstChild().toText().data();
                    if (field.tagName() == "ItemName")
                        fields["item_name"] = field.firstChild().toText().data();
                    if (field.tagName() == "ItemTypeID")
                        fields["item_type_id"] = field.firstChild().toText().data();
                    if (field.tagName() == "ColorID")
                        fields["color_id"] = field.firstChild().toText().data().toInt();
                    if (field.tagName() == "ItemTypeName")
                        fields["item_type"] = field.firstChild().toText().data();
                    if (field.tagName() == "ColorName")
                        fields["color_name"] = field.firstChild().toText().data();
                    if (field.tagName() == "CategoryID")
                        fields["category_id"] = field.firstChild().toText().data().toInt();
                    if (field.tagName() == "CategoryName")
                        fields["category_name"] = field.firstChild().toText().data();
                    if (field.tagName() == "Status")
                        fields["status"] = field.firstChild().toText().data();
                    if (field.tagName() == "Qty")
                        fields["quantity"] = field.firstChild().toText().data().toInt();
                    if (field.tagName() == "Price")
                        fields["unit_price"] = field.firstChild().toText().data().toDouble();
                    if (field.tagName() == "Condition")
                        fields["new_or_used"] = field.firstChild().toText().data();
                    if (field.tagName() == "OrigPrice")
                        fields["OrigPrice"] = field.firstChild().toText().data().toDouble();
                    if (field.tagName() == "OrigQty")
                        fields["OrigQty"] = field.firstChild().toText().data().toInt();

                    // Next field
                    field = field.nextSibling().toElement();
                }
            }

            QSqlError error = p_tableModel->addItemToTable(fields);

            // Next sibling
            item = item.nextSibling().toElement();
        }
        listModel = new ListModel(this, p_tableModel);
        ui->tabWidget->addTab(listModel, tableName);
    }
}
