#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordersdialog.h"
#include "settingsdialog.h"
#include "inventory.h"
#include "sqldatabase.h"
#include "categories.h"

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
    bricklink.importUserInventory();

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

void MainWindow::on_actionStore_Inventory_triggered()
{
    int numberOfTabs = ui->tabWidget->count();
    QList<QString> tabs;
    for(int i = 0; i < numberOfTabs; i++)
    {
        tabs.append(ui->tabWidget->tabText(i));
    }
    int index = tabs.indexOf("Store Inventory");
    if( index == -1) {
        Inventory *inv = new Inventory(this);
        ui->tabWidget->addTab(inv, "Store Inventory");
        ui->tabWidget->setCurrentIndex(numberOfTabs);
    } else {
        ui->tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::on_actionMy_Inventory_triggered()
{
    categories = new Categories(this);
    categories->exec();
}

void MainWindow::on_actionColors_triggered()
{
    categories = new Categories(this, "colors");
    categories->exec();
}

void MainWindow::on_actionCategories_triggered()
{
    categories = new Categories(this, "categories");
    categories->exec();
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
}

void MainWindow::openInventoryTab(QList<QString> orderIDs)
{
    int numberOfTabs = ui->tabWidget->count();
    QList<QString> tabs;
    for(int i = 0; i < numberOfTabs; i++)
    {
        tabs.append(ui->tabWidget->tabText(i));
    }
    foreach(QString orderID, orderIDs)
    {
        int numberOfTabs = ui->tabWidget->count();
        if(tabs.indexOf("Order#" + orderID) == -1)
        {
            Inventory *inv = new Inventory(this, orderID.toInt());
            ui->tabWidget->addTab(inv, "Order#" + orderID);
            ui->tabWidget->setCurrentIndex(numberOfTabs);
        }
    }
    ordersDialog->close();
}
