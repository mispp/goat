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
    src/CodeEditor.cpp \
    src/ConnectionManager.cpp \
    src/main.cpp \
    ui/ConnectionTab.cpp \
    src/ConnectionStandardItem.cpp \
    src/NewConnectionDialog.cpp

HEADERS  += \
    src/MainWindow.h \
    src/CodeEditor.h \
    src/ConnectionManager.h \
    ui/ConnectionTab.h \
    src/ConnectionStandardItem.h \
    src/NewConnectionDialog.h

FORMS    += \
    ui/MainWindow.ui \
    ui/ConnectionTab.ui \
    ui/NewConnectionDialog.ui

DISTFILES += \
    LICENSE \
    README.md
