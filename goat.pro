#-------------------------------------------------
#
# Project created by QtCreator 2016-07-08T23:58:37
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET = goat
TARGET.files = goat
TARGET.path = /usr/bin
INSTALLS += TARGET


SOURCES += \
    src/main.cpp \
    src/ConnectionManager.cpp \
    src/Connection.cpp \
    src/Csv.cpp \
    ui/TableView.cpp \
    ui/MainWindow.cpp \
    ui/CodeEditor.cpp \
    ui/Highlighter.cpp \
    ui/AboutDialog.cpp \
    ui/ConnectionWidget.cpp \
    ui/ConnectionManagerDialog.cpp \
    ui/QueryTab.cpp \
    src/QueryManager.cpp


HEADERS  += \
    src/ConnectionManager.h \
    src/Connection.h \
    src/Csv.h \
    ui/TableView.h \
    ui/MainWindow.h \
    ui/CodeEditor.h \
    ui/Highlighter.h \
    ui/AboutDialog.h \
    ui/ConnectionWidget.h \
    ui/ConnectionManagerDialog.h \
    ui/QueryTab.h \
    src/QueryManager.h

FORMS    += \
    ui/MainWindow.ui \
    ui/TableView.ui \
    ui/AboutDialog.ui \
    ui/ConnectionWidget.ui \
    ui/ConnectionManagerDialog.ui \
    ui/QueryTab.ui

DISTFILES += \
    LICENSE \
    README.md \
    CMakeLists.txt

RESOURCES += \
    resources/data.qrc \
    resources/icons.qrc

ICON.files = packaging/goat.png
ICON.path = /usr/share/icons/hicolor/16x16
INSTALLS += ICON

DESKTOP.files = packaging/goat.desktop
DESKTOP.path = /usr/share/applications
INSTALLS += DESKTOP
