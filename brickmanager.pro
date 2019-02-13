QT = core widgets network networkauth sql
requires(qtConfig(tableview))
CONFIG -= app_bundle

HEADERS += \
    bricklink.h \
    mainwindow.h \
    ordersdialog.h \
    settingsdialog.h \
    sqldatabase.h \
    datamodels.h \
    simplepopup.h \
    listmodel.h \
    colordelegate.h

SOURCES += \
    main.cpp \
    bricklink.cpp \
    mainwindow.cpp \
    ordersdialog.cpp \
    settingsdialog.cpp \
    sqldatabase.cpp \
    datamodels.cpp \
    listmodel.cpp \
    simplepopup.cpp \
    colordelegate.cpp

FORMS += \
    mainwindow.ui \
    ordersdialog.ui \
    inventory.ui \
    settingsdialog.ui \
    simplepopup.ui \
    listmodel.ui

DISTFILES += \
    README.md \
    LICENSE

