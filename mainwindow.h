#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ordersdialog.h"
#include "settingsdialog.h"
#include "labelsdialog.h"
#include "listmodel.h"
#include "bricklink.h"
#include "ctransfer.h"

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
    void insertItemIntoSheet(QMap<QString, QVariant> fields);

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_As_triggered();
    void on_actionOrders_triggered();
    void on_actionMy_Inventory_triggered();
    void on_actionExit_triggered();
    void on_actionAdd_Items_triggered();
    void on_actionUpdate_Database_triggered();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionClose_triggered();
    void on_actionManage_labels_triggered();

    void updateStatusBar(QString msg, int timeout);

private:
    Ui::MainWindow *ui;
    CTransfer *m_trans;
    OrdersDialog *ordersDialog;
    SettingsDialog *settingsDialog;
    LabelsDialog *labelsDialog;
    QMap<QString, ListModel*> tabList;
    BrickLink bricklink;
};

#endif // MAINWINDOW_H
