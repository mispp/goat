#include "CodeEditor.h"

#include "SqlUiLexer.h"
#include "Settings.h"

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QTextBlock>
#include <QShortcut>

SqlUiLexer* CodeEditor::sqlLexer = nullptr;

CodeEditor::CodeEditor(QWidget *parent) : ExtendedScintilla(parent)
{
    if(sqlLexer == nullptr)
    {
        sqlLexer = new SqlUiLexer(this);
    }

    setLexer(sqlLexer);

    // Set icons for auto completion
    registerImage(SqlUiLexer::ApiCompleterIconIdKeyword, QImage(":/icons/keyword"));
    registerImage(SqlUiLexer::ApiCompleterIconIdFunction, QImage(":/icons/function"));
    registerImage(SqlUiLexer::ApiCompleterIconIdTable, QImage(":/icons/table"));
    registerImage(SqlUiLexer::ApiCompleterIconIdColumn, QImage(":/icons/field"));

    reloadSettings();
}

CodeEditor::~CodeEditor()
{

}

void CodeEditor::reloadSettings()
{
    // Enable auto completion if it hasn't been disabled
    if(Settings::getValue("editor", "auto_completion").toBool())
    {
        setAutoCompletionThreshold(3);
        setAutoCompletionCaseSensitivity(true);
        setAutoCompletionShowSingle(true);
        setAutoCompletionSource(QsciScintilla::AcsAPIs);
    } else {
        setAutoCompletionThreshold(0);
    }

    ExtendedScintilla::reloadSettings();

    setupSyntaxHighlightingFormat(sqlLexer, "comment", QsciLexerSQL::Comment);
    setupSyntaxHighlightingFormat(sqlLexer, "comment", QsciLexerSQL::CommentLine);
    setupSyntaxHighlightingFormat(sqlLexer, "comment", QsciLexerSQL::CommentDoc);
    setupSyntaxHighlightingFormat(sqlLexer, "keyword", QsciLexerSQL::Keyword);
    setupSyntaxHighlightingFormat(sqlLexer, "table", QsciLexerSQL::KeywordSet6);
    setupSyntaxHighlightingFormat(sqlLexer, "function", QsciLexerSQL::KeywordSet7);
    setupSyntaxHighlightingFormat(sqlLexer, "string", QsciLexerSQL::DoubleQuotedString);
    setupSyntaxHighlightingFormat(sqlLexer, "string", QsciLexerSQL::SingleQuotedString);
    setupSyntaxHighlightingFormat(sqlLexer, "identifier", QsciLexerSQL::Identifier);
    setupSyntaxHighlightingFormat(sqlLexer, "identifier", QsciLexerSQL::QuotedIdentifier);
}

QString CodeEditor::getSelection()
{
    return selectedText();
}

QString CodeEditor::getQueryAtCursor()
{
    //https://stackoverflow.com/questions/50995743/qplaintextedit-qtextcursor-how-to-get-lines-above-and-below-cursor-until-abo

    return selectedText();
    /* this needs to be adjusted */

    /*
    QTextBlock block = textCursor().block();

    QTextBlock before = block;
    QTextBlock after = block;

    if(block.text().trimmed().isEmpty()) return "";

    QStringList lines;
    if (!block.text().trimmed().startsWith("--")) lines.append(block.text().trimmed());

    do
    {
        before = before.previous();
        if(before.text().trimmed().isEmpty()) break;
        if(!before.text().trimmed().startsWith("--")) lines.prepend(before.text());
    }
    while(before.isValid());

    do
    {
        after = after.next();
        if(after.text().trimmed().isEmpty()) break;
        if(!after.text().trimmed().startsWith("--")) lines.append(after.text());
    }
    while(after.isValid());

    return lines.join("\n");
    */
}
