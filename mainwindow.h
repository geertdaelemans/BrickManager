#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ordersdialog.h"
#include "settingsdialog.h"
#include "category.h"
#include "colors.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Category cat;
    Colors col;

public slots:
    void openInventoryTab(QList<QString>);

private slots:
    void on_actionOrders_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_tabWidget_tabCloseRequested(int index);

    void on_actionSettings_triggered();

private:
    Ui::MainWindow *ui;
    OrdersDialog *ordersDialog;
    SettingsDialog *settingsDialog;
};

#endif // MAINWINDOW_H
