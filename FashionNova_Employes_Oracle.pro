QT += widgets sql pdf

CONFIG += c++17
TEMPLATE = app
TARGET = FashionNova_Employes

SOURCES += \
    src/main.cpp \
    src/connexion.cpp \
    src/employe.cpp \
    src/logindialog.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/connexion.h \
    src/employe.h \
    src/logindialog.h \
    src/mainwindow.h

FORMS += src/logindialog.ui

RESOURCES += resources.qrc
