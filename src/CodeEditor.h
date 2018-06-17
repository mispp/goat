#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QTextEdit>
#include <QShortcut>

class CodeEditor : public QTextEdit
{
	Q_OBJECT


public:
	CodeEditor(QWidget *parent);
	//const QShortcut* m_shortcut;
	QString getSqlStatement();
};

#endif // CODEEDITOR_H
