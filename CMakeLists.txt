cmake_minimum_required(VERSION 3.20)
project(QGenetic)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Widgets
        Charts
        )

add_executable(QGenetic main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        boteditor.cpp boteditor.h boteditor.ui
        kled.h kled.cpp
        geneticworld.cpp geneticworld.h
        botstruct.h botstruct.cpp
        resources.qrc)

target_link_libraries(QGenetic
        Qt::Core
        Qt::Widgets
        Qt::Charts
        )