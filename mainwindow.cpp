#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordersdialog.h"
#include "settingsdialog.h"
#include "sqldatabase.h"
#include "listmodel.h"
#include "datamodels.h"
#include "simplepopup.h"

#include <QMessageBox>

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
    connect(&bricklink, SIGNAL(messageSent()), &loop, SLOT(quit()));
    loop.exec();

    // Hide message box
    p_popup->hide();

    // Prepare list
    listModel = new ListModel(this, Tables::userinventories);
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
    listModel = new ListModel(this, Tables::colors);
    listModel->exec();
}

void MainWindow::on_actionCategories_triggered()
{
    listModel = new ListModel(this, Tables::categories);
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
            connect(&bricklink, SIGNAL(messageSent()), &loop, SLOT(quit()));
            loop.exec();

            // Hide message box
            p_popup->hide();

            // Prepare list
            ListModel *inv = new ListModel(this, Tables::orderitem, orderID.toInt());
            ui->tabWidget->addTab(inv, "Order#" + orderID);
            ui->tabWidget->setCurrentIndex(numberOfTabs);
        }
    }
    ordersDialog->close();
}
