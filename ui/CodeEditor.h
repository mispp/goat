#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

#include "ui/Highlighter.h"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    QString getQueryAtCursor();

private:
    void highlightCurrentLine();
    Highlighter m_highlighter;

};

#endif // CODEEDITOR_H
