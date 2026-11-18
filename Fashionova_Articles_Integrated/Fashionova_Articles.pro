QT += core gui widgets sql printsupport
CONFIG += c++17
TARGET = Fashionova_Articles
msvc:QMAKE_CXXFLAGS += /utf-8
SOURCES += main.cpp mainwindow.cpp widgets.cpp article.cpp connection.cpp modulepage.cpp \
    STYLEHUB/modules/fashionova-v2/client.cpp \
    STYLEHUB/modules/fashionova-v2/clientsv2window.cpp \
    STYLEHUB/modules/fashionova-v2/statswidgets.cpp \
    STYLEHUB/src/employe.cpp STYLEHUB/src/employepage.cpp STYLEHUB/src/logindialog.cpp
HEADERS += mainwindow.h widgets.h article.h connection.h modulepage.h \
    STYLEHUB/modules/fashionova-v2/client.h \
    STYLEHUB/modules/fashionova-v2/clientsv2window.h \
    STYLEHUB/modules/fashionova-v2/statswidgets.h \
    STYLEHUB/src/employe.h STYLEHUB/src/employepage.h STYLEHUB/src/logindialog.h
FORMS += mainwindow.ui STYLEHUB/modules/fashionova-v2/clientsv2window.ui
RESOURCES += resources.qrc STYLEHUB/modules/fashionova-v2/resources_clientsv2.qrc
