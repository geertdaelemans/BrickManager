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
    category.h \
    settingsdialog.h \
    sqldatabase.h \
    categories.h

SOURCES += \
    main.cpp \
    bricklink.cpp \
    mainwindow.cpp \
    ordersdialog.cpp \
    inventory.cpp \
    inventorytablemodel.cpp \
    category.cpp \
    orderstablemodel.cpp \
    settingsdialog.cpp \
    sqldatabase.cpp \
    categories.cpp

FORMS += \
    mainwindow.ui \
    ordersdialog.ui \
    inventory.ui \
    settingsdialog.ui \
    categories.ui

DISTFILES += \
    README.md \
    LICENSE

