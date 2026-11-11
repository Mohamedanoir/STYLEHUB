QT += core gui widgets sql svg

INCLUDEPATH += src

CONFIG += c++17
TEMPLATE = app
TARGET = FashioNova_Integration

SOURCES += \
    main.cpp \
    connexion.cpp \
    maquette.cpp \
    mainwindow.cpp \
    statwidgets.cpp \
    icones.cpp \
    widgets.cpp \
    modulepage.cpp \
    employe.cpp \
    employepage.cpp \
    src/machine.cpp \
    src/machinespage.cpp \
    src/database.cpp \
    src/uihelpers.cpp \
    src/chartwidgets.cpp \
    src/metiers.cpp \
    src/recorddialogs.cpp \
    src/bannerwidget.cpp \
    src/sidebarwidget.cpp \
    modules/fashionova-v2/client.cpp \
    modules/fashionova-v2/clientsv2window.cpp \
    modules/fashionova-v2/statswidgets.cpp \
    logindialog.cpp

HEADERS += \
    connexion.h \
    maquette.h \
    mainwindow.h \
    statwidgets.h \
    icones.h \
    widgets.h \
    modulepage.h \
    employe.h \
    employepage.h \
    src/machine.h \
    src/machinevalidation.h \
    src/machinespage.h \
    src/database.h \
    src/uihelpers.h \
    src/chartwidgets.h \
    src/metiers.h \
    src/recorddialogs.h \
    src/businessrecords.h \
    src/bannerwidget.h \
    src/sidebarwidget.h \
    modules/fashionova-v2/client.h \
    modules/fashionova-v2/clientsv2window.h \
    modules/fashionova-v2/statswidgets.h \
    logindialog.h

RESOURCES += \
    resources.qrc \
    modules/fashionova-v2/resources_clientsv2.qrc

FORMS += \
    mainwindow.ui \
    src/machinespage.ui \
    modules/fashionova-v2/clientsv2window.ui
