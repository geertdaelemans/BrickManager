#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordersdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOrders_triggered()
{
    OrdersDialog ordersDialog;
    ordersDialog.setModal(true);
    ordersDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    ordersDialog.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Books"),
            tr("<p>The <b>BrickLink Manager</b> application is being developed by Geert Daelemans."));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}
