QT = core widgets network networkauth
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
    colors.h \
    settingsdialog.h

SOURCES += \
    main.cpp \
    bricklink.cpp \
    mainwindow.cpp \
    ordersdialog.cpp \
    inventory.cpp \
    inventorytablemodel.cpp \
    category.cpp \
    orderstablemodel.cpp \
    colors.cpp \
    settingsdialog.cpp

FORMS += \
    mainwindow.ui \
    ordersdialog.ui \
    inventory.ui \
    settingsdialog.ui

DISTFILES += \
    README.md \
    LICENSE

