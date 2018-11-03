#-------------------------------------------------
#
# Project created by QtCreator 2016-07-08T23:58:37
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    src/ConnectionManager.cpp \
    src/Connection.cpp \
    src/Csv.cpp \
    src/TableView.cpp \
    src/MainWindow.cpp \
    src/CodeEditor.cpp \
    src/Highlighter.cpp \
    src/AboutDialog.cpp \
    src/ConnectionWidget.cpp \
    src/ConnectionManagerDialog.cpp \
    src/QueryTab.cpp \
    src/StringUtils.cpp \
    src/Query.cpp \
    src/QueryExporter.cpp \
    src/QueryStopper.cpp \
    src/AbstractQuery.cpp \
    src/ExportQueryDialog.cpp \
    src/DataFormatter.cpp


HEADERS  += \
    src/ConnectionManager.h \
    src/Connection.h \
    src/Csv.h \
    src/TableView.h \
    src/MainWindow.h \
    src/CodeEditor.h \
    src/Highlighter.h \
    src/AboutDialog.h \
    src/ConnectionWidget.h \
    src/ConnectionManagerDialog.h \
    src/QueryTab.h \
    src/StringUtils.h \
    src/Query.h \
    src/Row.h \
    src/RowSet.h \
    src/QueryExporter.h \
    src/QueryStopper.h \
    src/AbstractQuery.h \
    src/ExportQueryDialog.h \
    src/DataFormatter.h


FORMS    += \
    src/MainWindow.ui \
    src/TableView.ui \
    src/AboutDialog.ui \
    src/ConnectionWidget.ui \
    src/ConnectionManagerDialog.ui \
    src/QueryTab.ui \
    src/ExportQueryDialog.ui

DISTFILES += \
    LICENSE \
    README.md \
    CMakeLists.txt

RESOURCES += \
    resources/data.qrc \
    resources/icons.qrc

test {

    TEMPLATE = app
    TARGET = tests

    QT += testlib

    HEADERS += \
		test/TestStringUtils.h

    SOURCES += \
	    test/TestStringUtils.cpp \
			test/test_main.cpp

} else {

	TEMPLATE = app

	TARGET = goat
	TARGET.files = goat
	TARGET.path = /usr/bin
	INSTALLS += TARGET

	SOURCES += \
		src/main.cpp

	ICON.files = packaging/goat.png
	ICON.path = /usr/share/icons/hicolor/16x16
	INSTALLS += ICON

	DESKTOP.files = packaging/goat.desktop
	DESKTOP.path = /usr/share/applications
	INSTALLS += DESKTOP

}

