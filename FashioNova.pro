QT += core gui widgets sql

CONFIG += c++17

TARGET = FashioNova
TEMPLATE = app

SOURCES += \
    main.cpp \
    connexion.cpp \
    maquette.cpp \
    statwidgets.cpp \
    icones.cpp \
    widgets.cpp \
    modulepage.cpp \
    mainwindow.cpp

HEADERS += \
    connexion.h \
    maquette.h \
    statwidgets.h \
    icones.h \
    widgets.h \
    modulepage.h \
    mainwindow.h

RESOURCES += resources.qrc

FORMS += \
    mainwindow.ui
