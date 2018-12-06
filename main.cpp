#include "mainwindow.h"

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
//    mainWindow.setWindowState(Qt::Maximized);
    mainWindow.showMaximized();

    return app.exec();
}
