#include "mainwindow.h"
#include "sqldatabase.h"

#include <QApplication>
#include <QWindow>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    app.setApplicationName("Brick Manager");
    app.setApplicationDisplayName("Brick Manager");
    app.setOrganizationDomain("xodarap.be");
    app.setOrganizationName("Studs Everywhere");

    MainWindow mainWindow;
    mainWindow.showMaximized();

    return app.exec();
}
