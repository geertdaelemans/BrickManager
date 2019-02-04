#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ordersdialog.h"
#include "settingsdialog.h"
#include "categories.h"
#include "bricklink.h"

#include <QMainWindow>
#include <QtSql>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openInventoryTab(QList<QString>);

private slots:
    void on_actionOrders_triggered();
    void on_actionStore_Inventory_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionSettings_triggered();

    void on_actionCategories_triggered();

private:
    Ui::MainWindow *ui;
    OrdersDialog *ordersDialog;
    SettingsDialog *settingsDialog;
    Categories *categories;
    BrickLink bricklink;
};

#endif // MAINWINDOW_H
