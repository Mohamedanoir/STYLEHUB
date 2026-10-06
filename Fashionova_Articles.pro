QT += core gui widgets
CONFIG += c++17
TARGET = Fashionova_Articles
msvc:QMAKE_CXXFLAGS += /utf-8
SOURCES += main.cpp mainwindow.cpp widgets.cpp
HEADERS += mainwindow.h widgets.h
FORMS += mainwindow.ui
RESOURCES += resources.qrc
