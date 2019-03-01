QT = core widgets network networkauth sql xml
requires(qtConfig(tableview))
CONFIG -= app_bundle

HEADERS += \
    bricklink.h \
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
    additemdialog.h

SOURCES += \
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
    additemdialog.cpp

FORMS += \
    mainwindow.ui \
    ordersdialog.ui \
    inventory.ui \
    settingsdialog.ui \
    simplepopup.ui \
    listmodel.ui \
    additemdialog.ui

DISTFILES += \
    README.md \
    LICENSE

RESOURCES += \
    images.qrc \
    xml.qrc

