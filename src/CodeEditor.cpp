#include "CodeEditor.h"
#include <QPlainTextEdit>
#include <QWidget>
#include <QShortcut>
#include <QDebug>
#include <QTextCursor>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent) : QTextEdit(parent)
{
    this->setStyleSheet("QTextEdit { font-family: Courier }");
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

