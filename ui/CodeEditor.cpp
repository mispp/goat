#include "CodeEditor.h"

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QTextBlock>
#include <QShortcut>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setStyleSheet("CodeEditor { font: 18px; }");
    setTabStopDistance(fontMetrics().width(' ') * 4);

    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    //connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this), SIGNAL(activated()), parent, SLOT(on_ctrlEnter_triggered()));

    m_highlighter.setDocument(document());

    highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{

}

void CodeEditor::highlightCurrentLine()
{
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor("#F0F0E4"));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

QString CodeEditor::getQueryAtCursor()
{
    //https://stackoverflow.com/questions/50995743/qplaintextedit-qtextcursor-how-to-get-lines-above-and-below-cursor-until-abo

    QTextBlock block = textCursor().block();

    QTextBlock before = block;
    QTextBlock after = block;

    if(block.text().trimmed().isEmpty()) return "";

    QStringList lines{block.text()};

    do
    {
        before = before.previous();
        if(before.text().trimmed().isEmpty()) break;
        lines.prepend(before.text());
    }
    while(before.isValid());

    do
    {
        after = after.next();
        if(after.text().trimmed().isEmpty()) break;
        lines.append(after.text());
    }
    while(after.isValid());

    return lines.join("\n");
}
