#-------------------------------------------------
#
# Project created by QtCreator 2016-07-08T23:58:37
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Goat
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/ConnectionStandardItem.cpp \
    src/ConnectionManager.cpp \
    ui/MainWindow.cpp \
    ui/ConnectionManagerDialog.cpp \
    ui/ConnectionTab.cpp \
    ui/CodeEditor.cpp \
    ui/Highlighter.cpp

HEADERS  += \
    src/ConnectionStandardItem.h \
    src/ConnectionManager.h \
    ui/ConnectionManagerDialog.h \
    ui/MainWindow.h \
    ui/ConnectionTab.h \
    ui/CodeEditor.h \
    ui/Highlighter.h

FORMS    += \
    ui/MainWindow.ui \
    ui/ConnectionTab.ui \
    ui/ConnectionManagerDialog.ui

DISTFILES += \
    LICENSE \
    README.md \
    CMakeLists.txt

RESOURCES += \
    resources/data.qrc

