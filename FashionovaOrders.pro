QT += core gui widgets network webenginewidgets printsupport sql

CONFIG += c++17

TARGET = FashionovaOrders
TEMPLATE = app

HEADERS += \
    AnimatedBotButton.h \
    commande.h \
    connection.h \
    mainwindow.h \
    connection.h

SOURCES += \
    commande.cpp \
    connection.cpp \
    main.cpp \
    mainwindow.cpp \
    connection.cpp

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    api_key.txt \
    README.txt