cmake_minimum_required(VERSION 3.16)
project(STYLEHUB_Fashionova VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network PrintSupport Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network PrintSupport Svg)

add_executable(FashioNova
    main.cpp
    deliverymap.cpp deliverymap.h

    # Authentification (affichée avant l'application)
    login/loginwindow.cpp login/loginwindow.h login/loginwindow.ui
    login/login.qrc
    mainwindow.cpp mainwindow.h

    # Maquettes: UI + custom widgets/model already present in integration
    integrated/maquettes/maquettespage.cpp integrated/maquettes/maquettespage.h integrated/maquettes/maquettespage.ui
    maquette.cpp maquette.h
    statwidgets.cpp statwidgets.h
    widgets.cpp widgets.h
    icones.cpp icones.h
    connexion.cpp connexion.h

    # Clients
    integrated/clients/clientspage.cpp integrated/clients/clientspage.h integrated/clients/clientspage.ui

    # Employes
    integrated/employes/employespage.cpp integrated/employes/employespage.h
    integrated/employes/logindialog.cpp integrated/employes/logindialog.h

    # Articles
    integrated/articles/articlespage.cpp integrated/articles/articlespage.h integrated/articles/articlespage.ui
    integrated/articles/articlewidgets.cpp integrated/articles/articlewidgets.h

    # Machines
    modules/machines/src/machinespage.cpp modules/machines/src/machinespage.h modules/machines/src/machinespage.ui
    modules/machines/src/detailsdialog.cpp modules/machines/src/detailsdialog.h modules/machines/src/detailsdialog.ui
    modules/machines/src/database.cpp modules/machines/src/database.h
    modules/machines/src/uihelpers.cpp modules/machines/src/uihelpers.h
    modules/machines/src/chartwidgets.cpp modules/machines/src/chartwidgets.h
    modules/machines/src/bannerwidget.cpp modules/machines/src/bannerwidget.h
    modules/machines/src/machine.h

    resources.qrc
    modules/clients/resources.qrc
    modules/employes/resources.qrc
    modules/articles/resources.qrc
    modules/machines/resources.qrc
)

target_include_directories(FashioNova PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/login
    ${CMAKE_CURRENT_SOURCE_DIR}/integrated/clients
    ${CMAKE_CURRENT_SOURCE_DIR}/integrated/employes
    ${CMAKE_CURRENT_SOURCE_DIR}/integrated/articles
    ${CMAKE_CURRENT_SOURCE_DIR}/integrated/maquettes
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/machines/src
)

target_link_libraries(FashioNova PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Svg
)

if(MSVC)
    target_compile_options(FashioNova PRIVATE /utf-8)
endif()

set_target_properties(FashioNova PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
