#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QShortcut>

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT


public:
	CodeEditor(QWidget *parent);
	//const QShortcut* m_shortcut;
	QString getSqlStatement();
};

#endif // CODEEDITOR_H
