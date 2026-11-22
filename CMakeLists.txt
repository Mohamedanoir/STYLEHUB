cmake_minimum_required(VERSION 3.16)
project(GestionMachines LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/forms")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Svg)

add_executable(GestionMachines
    src/machine.cpp
    src/machine.h
    src/machine.ui
    src/connection.cpp
    src/connection.h
    forms/mainwindow.ui
    forms/detailsdialog.ui
    resources.qrc
)

target_include_directories(GestionMachines PRIVATE src forms)
target_link_libraries(GestionMachines PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Svg
)
