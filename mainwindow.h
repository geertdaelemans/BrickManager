#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ordersdialog.h"
#include "settingsdialog.h"
#include "listmodel.h"
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
    int addTab(ListModel *page, const QString &label);
    void removeTab(int index);

public slots:
    void openInventoryTab(QList<QString>);
    void insertItemIntoSheet(QList<QString>);

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionOrders_triggered();
    void on_actionMy_Inventory_triggered();
    void on_actionExit_triggered();
    void on_actionAdd_Items_triggered();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionUpdate_Database_triggered();

private:
    Ui::MainWindow *ui;
    OrdersDialog *ordersDialog;
    SettingsDialog *settingsDialog;
    QMap<QString, ListModel*> tabList;
    BrickLink bricklink;
};

#endif // MAINWINDOW_H
