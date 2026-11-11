 QT += widgets sql

CONFIG += c++17
TEMPLATE = app
TARGET = HighFashionWorkshop

# Source en UTF-8 (accents) pour MSVC
msvc: QMAKE_CXXFLAGS += /utf-8

SOURCES += main.cpp \
           connection.cpp \
           client.cpp \
           mainwindow.cpp \
           statswidgets.cpp

HEADERS += mainwindow.h \
           connection.h \
           client.h \
           statswidgets.h

FORMS += mainwindow.ui

RESOURCES += resources.qrc
