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
    src/MainWindow.cpp \
    src/NewConnectionDialog.cpp \
    src/CodeEditor.cpp \
    src/ConnectionManager.cpp \
    src/Connection.cpp \
    src/main.cpp \
    src/StaticFunctions.cpp \
    ui/ConnectionTab.cpp

HEADERS  += \
    src/MainWindow.h \
    src/NewConnectionDialog.h \
    src/CodeEditor.h \
    src/ConnectionManager.h \
    src/Connection.h \
    src/StaticFunctions.h \
    ui/ConnectionTab.h

FORMS    += \
    ui/MainWindow.ui \
    ui/NewConnectionDialog.ui \
    ui/ConnectionTab.ui

DISTFILES += \
    LICENSE \
    README.md
