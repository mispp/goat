#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

#include "ExtendedScintilla.h"

class FindReplaceDialog;
class SqlUiLexer;

class CodeEditor : public ExtendedScintilla
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);
    virtual ~CodeEditor();

    static SqlUiLexer* sqlLexer;


    QString getQueryAtCursor();
    QString getSelection();


public slots:
    void reloadSettings();

private:
    //void highlightCurrentLine();

};

#endif // CODEEDITOR_H
