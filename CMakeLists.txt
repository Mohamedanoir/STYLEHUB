cmake_minimum_required(VERSION 3.16)

project(FashioNova VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    connexion.h connexion.cpp
    maquette.h maquette.cpp
    statwidgets.h statwidgets.cpp
    icones.h icones.cpp
    widgets.h widgets.cpp
    mainwindow.h mainwindow.cpp mainwindow.ui
    resources.qrc
)

add_executable(FashioNova ${PROJECT_SOURCES})

# pour que ui_mainwindow.h trouve widgets.h et statwidgets.h
target_include_directories(FashioNova PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FashioNova PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

set_target_properties(FashioNova PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
