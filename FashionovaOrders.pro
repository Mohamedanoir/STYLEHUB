QT += widgets webenginewidgets webchannel network printsupport
CONFIG += c++17
TEMPLATE = app
TARGET = FashionovaOrders

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += resources.qrc

win32:CONFIG += windows