QT += core gui widgets sql
CONFIG += c++17
TARGET = Fashionova_Articles
msvc:QMAKE_CXXFLAGS += /utf-8
SOURCES += main.cpp mainwindow.cpp widgets.cpp article.cpp connection.cpp
HEADERS += mainwindow.h widgets.h article.h connection.h
FORMS += mainwindow.ui
RESOURCES += resources.qrc
