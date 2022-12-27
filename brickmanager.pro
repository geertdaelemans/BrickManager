QT = core widgets network networkauth sql xml
requires(qtConfig(tableview))
CONFIG -= app_bundle

HEADERS += \
    bricklink.h \
    cdocument.h \
    config.h \
    container.h \
    cref.h \
    exportxml.h \
    framework.h \
    labelsdialog.h \
    mainwindow.h \
    ordersdialog.h \
    settingsdialog.h \
    sqldatabase.h \
    simplepopup.h \
    listmodel.h \
    listmodeldelegate.h \
    generictablemodel.h \
    datamodel.h \
    checkboxdelegate.h \
    additemdialog.h \
    progressdialog.h \
    curllabel.h \
    lzmadec.h \
    ctransfer.h

SOURCES += \
    cdocument.cpp \
    config.cpp \
    cref.cpp \
    exportxml.cpp \
    framework.cpp \
    labelsdialog.cpp \
    main.cpp \
    bricklink.cpp \
    mainwindow.cpp \
    ordersdialog.cpp \
    settingsdialog.cpp \
    sqldatabase.cpp \
    listmodel.cpp \
    simplepopup.cpp \
    listmodeldelegate.cpp \
    generictablemodel.cpp \
    datamodel.cpp \
    checkboxdelegate.cpp \
    additemdialog.cpp \
    progressdialog.cpp \
    curllabel.cpp \
    lzmadec.c \
    ctransfer.cpp

FORMS += \
    labelsdialog.ui \
    mainwindow.ui \
    ordersdialog.ui \
    inventory.ui \
    settingsdialog.ui \
    simplepopup.ui \
    listmodel.ui \
    additemdialog.ui \
    progressdialog.ui

DISTFILES += \
    README.md \
    LICENSE

RESOURCES += \
    images.qrc \
    auth.qrc

unix|win32: contains(QT_ARCH, i386) {
    LIBS += -L$$PWD/../../libs/curl-7.87.0-win32-mingw/lib/ -llibcurl.dll
    INCLUDEPATH += $$PWD/../../libs/curl-7.87.0-win32-mingw/include
    DEPENDPATH += $$PWD/../../libs/curl-7.87.0-win32-mingw/include
}else {
    LIBS += -L$$PWD/../../libs/curl-7.87.0-win64-mingw/lib/ -llibcurl.dll
    INCLUDEPATH += $$PWD/../../libs/curl-7.87.0-win64-mingw/include
    DEPENDPATH += $$PWD/../../libs/curl-7.87.0-win64-mingw/include
}
