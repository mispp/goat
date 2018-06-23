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
    src/ConnectionManager.cpp \
    src/main.cpp \
    ui/ConnectionTab.cpp \
    src/ConnectionStandardItem.cpp \
    ui/Highlighter.cpp \
    src/ConnectionManagerDialog.cpp

HEADERS  += \
    src/MainWindow.h \
    src/ConnectionManager.h \
    ui/ConnectionTab.h \
    src/ConnectionStandardItem.h \
    ui/Highlighter.h \
    src/ConnectionManagerDialog.h

FORMS    += \
    ui/MainWindow.ui \
    ui/ConnectionTab.ui \
    ui/ConnectionManagerDialog.ui

DISTFILES += \
    LICENSE \
    README.md
