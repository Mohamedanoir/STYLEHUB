QT += core gui widgets sql

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
    logindialog.h

RESOURCES += resources.qrc

FORMS += \
    mainwindow.ui
