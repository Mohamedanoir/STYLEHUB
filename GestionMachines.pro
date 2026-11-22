QT += core gui widgets sql svg
CONFIG += c++17
TEMPLATE = app
TARGET = GestionMachines

INCLUDEPATH += src forms

SOURCES += \
    src/machine.cpp \
    src/connection.cpp

HEADERS += \
    src/machine.h \
    src/connection.h

FORMS += \
    src/machine.ui \
    forms/mainwindow.ui \
    forms/detailsdialog.ui

RESOURCES += resources.qrc
