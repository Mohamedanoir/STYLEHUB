QT       += core gui widgets sql svg
CONFIG   += c++17
TARGET    = STYLEHUB
TEMPLATE  = app

msvc: QMAKE_CXXFLAGS += /utf-8

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/machinespage.cpp \
    src/employespage.cpp \
    src/logindialog.cpp \
    src/sidebarwidget.cpp \
    src/detailsdialog.cpp \
    src/database.cpp \
    src/uihelpers.cpp \
    src/chartwidgets.cpp \
    src/bannerwidget.cpp \
    src/metiers.cpp

HEADERS += \
    src/machine.h \
    src/mainwindow.h \
    src/machinespage.h \
    src/employespage.h \
    src/logindialog.h \
    src/sidebarwidget.h \
    src/detailsdialog.h \
    src/database.h \
    src/uihelpers.h \
    src/chartwidgets.h \
    src/bannerwidget.h \
    src/metiers.h

FORMS += \
    src/mainwindow.ui \
    src/machinespage.ui \
    src/detailsdialog.ui

RESOURCES += resources.qrc
