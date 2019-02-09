QT = core widgets network networkauth sql
requires(qtConfig(tableview))
CONFIG -= app_bundle

HEADERS += \
    bricklink.h \
    mainwindow.h \
    ordersdialog.h \
    orderstablemodel.h \
    inventory.h \
    inventorytablemodel.h \
    settingsdialog.h \
    sqldatabase.h \
    datamodels.h \
    simplepopup.h \
    listmodel.h

SOURCES += \
    main.cpp \
    bricklink.cpp \
    mainwindow.cpp \
    ordersdialog.cpp \
    inventory.cpp \
    inventorytablemodel.cpp \
    orderstablemodel.cpp \
    settingsdialog.cpp \
    sqldatabase.cpp \
    datamodels.cpp \
    listmodel.cpp \
    simplepopup.cpp

FORMS += \
    mainwindow.ui \
    ordersdialog.ui \
    inventory.ui \
    settingsdialog.ui \
    categories.ui \
    simplepopup.ui

DISTFILES += \
    README.md \
    LICENSE

