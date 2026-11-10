QT += widgets sql

CONFIG += c++17
TEMPLATE = app
TARGET = FashionNova_Employes

SOURCES += \
    src/main.cpp \
    src/connexion.cpp \
    src/employe.cpp \
    src/mainwindow.cpp \
    src/logindialog.cpp

HEADERS += \
    src/connexion.h \
    src/employe.h \
    src/mainwindow.h \
    src/logindialog.h

RESOURCES += resources.qrc
