#include "CodeEditor.h"
#include <QPlainTextEdit>
#include <QWidget>
#include <QShortcut>
#include <QDebug>
#include <QTextCursor>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
	this->setStyleSheet("QPlainTextEdit { font-family: Courier }");
	this->appendPlainText("select * from information_schema.tables");
	this->appendPlainText("");
	this->appendPlainText("");
	this->appendPlainText("select");
	this->appendPlainText("*");
	this->appendPlainText("from");
	this->appendPlainText("information_schema.tables");

	m_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);


	/*

		https://wiki.qt.io/New_Signal_Slot_Syntax
		new connect syntax:
		connect(sender, &Sender::valueChanged, receiver, &Receiver::updateValue );
		connect(sender, &Sender::valueChanged, someFunction);

	*/

	//connect(m_shortcut,		SIGNAL(activated()), this		, SLOT(on_ctrlEnter_triggered())	);
	connect(m_shortcut,		SIGNAL(activated()), parent		, SLOT(on_ctrlEnter_triggered())	);

}

QString CodeEditor::getSqlStatement()
{
	QTextCursor original_cursor_position = this->textCursor();
	QTextCursor cursorBefore = original_cursor_position;
	QTextCursor cursorAfter = original_cursor_position;

	QString before = "";
	QString at = "";
	QString after = "";

	at = original_cursor_position.block().text().trimmed();

	while (cursorBefore.block().previous().text().trimmed() != "")
	{
		cursorBefore.movePosition(QTextCursor::Up);
		before = cursorBefore.block().text() + " " + before;
	}

	while (cursorAfter.block().next().text().trimmed() != "")
	{
		cursorAfter.movePosition(QTextCursor::Down);
		after = after + " " + cursorAfter.block().text();
	}

	return before + " " + at + " " + after;
}

