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
    mainWindow.showMaximized();

    qDebug() << "SSL checks";
    qDebug() << "==========";
    qDebug() << "sslLibraryBuildVersionString" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "supportsSsl" << QSslSocket::supportsSsl();
    qDebug() << "sslLibraryVersionString" << QSslSocket::sslLibraryVersionString();

    return app.exec();
}
