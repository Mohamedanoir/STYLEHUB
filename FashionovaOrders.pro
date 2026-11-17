QT += core gui widgets network webenginewidgets printsupport sql svg

CONFIG += c++17

TARGET = FashionovaOrders
TEMPLATE = app

HEADERS += \
    AnimatedBotButton.h \
    commande.h \
    connection.h \
    connexion.h \
    employe.h \
    employepage.h \
    icones.h \
    logindialog.h \
    mainwindow.h \
    maquette.h \
    modulepage.h \
    statwidgets.h \
    widgets.h

SOURCES += \
    commande.cpp \
    connection.cpp \
    connexion.cpp \
    employe.cpp \
    employepage.cpp \
    icones.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    maquette.cpp \
    modulepage.cpp \
    statwidgets.cpp \
    widgets.cpp

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    api_key.txt \
    README.txt