QT = core widgets network networkauth
requires(qtConfig(tableview))
CONFIG -= app_bundle

HEADERS += \
    bricklink.h \
    mainwindow.h \
    ordersdialog.h \
    orderstablemodel.h

SOURCES += \
    main.cpp \
    bricklink.cpp \
    mainwindow.cpp \
    ordersdialog.cpp \
    orderstalblemodel.cpp

FORMS += \
    mainwindow.ui \
    ordersdialog.ui

