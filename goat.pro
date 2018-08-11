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
    src/ConnectionManager.cpp \
    src/Connection.cpp \
    ui/MainWindow.cpp \
    ui/CodeEditor.cpp \
    ui/Highlighter.cpp \
    ui/AboutDialog.cpp \
    ui/ConnectionDialog.cpp \
    ui/ConnectionManagerDialog.cpp \
    ui/QueryTab.cpp


HEADERS  += \
    src/ConnectionManager.h \
    src/Connection.h \
    ui/MainWindow.h \
    ui/CodeEditor.h \
    ui/Highlighter.h \
    ui/AboutDialog.h \
    ui/ConnectionDialog.h \
    ui/ConnectionManagerDialog.h \
    ui/QueryTab.h

FORMS    += \
    ui/MainWindow.ui \
    ui/AboutDialog.ui \
    ui/ConnectionDialog.ui \
    ui/ConnectionManagerDialog.ui \
    ui/QueryTab.ui

DISTFILES += \
    LICENSE \
    README.md \
    CMakeLists.txt

RESOURCES += \
    resources/data.qrc \
    resources/icons.qrc

