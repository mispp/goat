cmake_minimum_required(VERSION 3.13.4)

find_package(Qt5 5.12.2 REQUIRED COMPONENTS Core Gui Sql Widgets)
find_package(Qt5Sql REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(goat
    src/main.cpp
    src/ConnectionManager.cpp
    src/Connection.cpp
    src/Csv.cpp
    ui/TableView.cpp
    ui/MainWindow.cpp
    ui/CodeEditor.cpp
    ui/Highlighter.cpp
    ui/AboutDialog.cpp
    ui/ConnectionWidget.cpp
    ui/ConnectionManagerDialog.cpp
    ui/QueryTab.cpp
    src/StringUtils.cpp
    src/Query.cpp
    src/QueryExporter.cpp
    src/QueryStopper.cpp
    src/AbstractQuery.cpp
    ui/ExportQueryDialog.cpp
    src/DataFormatter.cpp
    src/ConnectionManager.h
    src/Connection.h
    src/Csv.h
    ui/TableView.h
    ui/MainWindow.h
    ui/CodeEditor.h
    ui/Highlighter.h
    ui/AboutDialog.h
    ui/ConnectionWidget.h
    ui/ConnectionManagerDialog.h
    ui/QueryTab.h
    src/StringUtils.h
    src/Query.h
    src/Row.h
    src/RowSet.h
    src/QueryExporter.h
    src/QueryStopper.h
    src/AbstractQuery.h
    ui/ExportQueryDialog.h
    src/DataFormatter.h
    ui/MainWindow.ui
    ui/TableView.ui
    ui/AboutDialog.ui
    ui/ConnectionWidget.ui
    ui/ConnectionManagerDialog.ui
    ui/QueryTab.ui
    ui/ExportQueryDialog.ui
    resources/data.qrc
    resources/icons.qrc
)

add_executable(Goat ${SOURCES}
    ${goat}
)

target_link_libraries(
    Goat
    Qt5::Core
    Qt5::Gui
    Qt5::Sql
    Qt5::Widgets
)
