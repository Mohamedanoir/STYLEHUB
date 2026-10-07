QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = HighFashionWorkshop

# Source en UTF-8 (accents) pour MSVC
msvc: QMAKE_CXXFLAGS += /utf-8

SOURCES += main.cpp \
           mainwindow.cpp \
           articlespage.cpp \
           widgets.cpp

HEADERS += mainwindow.h \
           articlespage.h \
           widgets.h

FORMS += mainwindow.ui \
         articlespage.ui

RESOURCES += resources.qrc
