TEMPLATE = app

QT += core gui sql network widgets xml concurrent

TARGET = goat

CONFIG += debug
CONFIG += qt
CONFIG += warn_on

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    main.cpp \
    core/ConnectionStandardItem.cpp \
    core/ConnectionManager.cpp \
    ui/MainWindow.cpp \
    ui/ConnectionManagerDialog.cpp \
    ui/ConnectionTab.cpp \
    ui/CodeEditor.cpp \
    ui/Highlighter.cpp \
    ui/ExtendedScintilla.cpp \
    ui/FindReplaceDialog.cpp \
    ui/Settings.cpp \
    ui/SqlUiLexer.cpp

HEADERS  += \
    core/ConnectionStandardItem.h \
    core/ConnectionManager.h \
    ui/ConnectionManagerDialog.h \
    ui/MainWindow.h \
    ui/ConnectionTab.h \
    ui/CodeEditor.h \
    ui/Highlighter.h \
    ui/ExtendedScintilla.h \
    ui/FindReplaceDialog.h \
    ui/Settings.h \
    ui/SqlUiLexer.h

FORMS    += \
    ui/MainWindow.ui \
    ui/ConnectionTab.ui \
    ui/ConnectionManagerDialog.ui \
    ui/FindReplaceDialog.ui

DISTFILES += \
    LICENSE \
    README.md \
    CMakeLists.txt

RESOURCES += \
    icons/icons.qrc

LIBPATH_QSCINTILLA=$$OUT_PWD/../libs/qscintilla/Qt4Qt5

unix {
    LIBS += -ldl
}

INCLUDEPATH += $$PWD/../libs/qscintilla/Qt4Qt5 $$PWD/..
LIBS += -L$$LIBPATH_QSCINTILLA -lqscintilla2
DEPENDPATH += $$PWD/../libs/qscintilla/Qt4Qt5

