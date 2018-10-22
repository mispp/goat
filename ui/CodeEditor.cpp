#include "CodeEditor.h"

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QTextBlock>
#include <QShortcut>
#include <QSettings>
#include <QAbstractItemView>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setStyleSheet("CodeEditor { font: 18px; }");
    setTabStopWidth(fontMetrics().width(' ') * 4);

    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    multilineCommentExpression.setPattern("/\\*+[^*]*\\*+(?:[^/*][^*]*\\*+)*/");
    singlelineCommentExpression.setPattern("--[^\n]*");
    m_highlighter.setDocument(document());
    highlightCurrentLine();

    QStringList list;
    QSettings settings(":/syntax/syntax-highlight/sql.ini", QSettings::IniFormat, this);
    if (settings.value("4/TYPE").toString() == "KEYWORD")
    {
        list = settings.value("4/ITEMS").toStringList();
    }

    m_completerModel = new QStringListModel(list);
    m_completer.setWidget(this);
    m_completer.setModel(m_completerModel);
    m_completer.setCaseSensitivity(Qt::CaseInsensitive);
    m_completer.setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_completer.setCompletionMode(QCompleter::PopupCompletion);
    m_completer.setWrapAround(false);
    QObject::connect(&m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
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

QString CodeEditor::getSelection()
{
    return textCursor().selectedText().replace("\u2029", "\n", Qt::CaseInsensitive).replace(multilineCommentExpression, "").replace(singlelineCommentExpression, "");
}

QString CodeEditor::getQueryAtCursor()
{
    //https://stackoverflow.com/questions/50995743/qplaintextedit-qtextcursor-how-to-get-lines-above-and-below-cursor-until-abo

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

    return lines.join("\n").replace(multilineCommentExpression, "").replace(singlelineCommentExpression, "");
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::insertCompletion(const QString &completion)
{
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    tc.removeSelectedText();
    tc.insertText(completion + " ");
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    static int minAutoCompleteSize = 2;
    static QString endOfWord("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-= \t\n");

    QCompleter *c = &m_completer;

    if (!c)
    {
        QPlainTextEdit::keyPressEvent(e);
        return;
    }

    bool popupVisible = c->popup()->isVisible();
    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);

    if (popupVisible)
    {
        // The following keys are forwarded by the completer to the widget
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    QPlainTextEdit::keyPressEvent(e);

    QString completionPrefix = textUnderCursor();
    QString oldPrefix = c->completionPrefix();
    c->setCompletionPrefix(completionPrefix);
    c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    QRect cr = cursorRect();
    cr.setWidth(200);

    bool hasCompletions = c->completionCount() != 0;
    bool isEndOfWord = endOfWord.contains(e->text().right(1));
    bool prefixChanged = oldPrefix != completionPrefix;

    bool shouldHide = popupVisible && (!hasCompletions || isEndOfWord);
    bool shouldShow = !popupVisible
            && hasCompletions
            && (
                isShortcut
                || (
                    !isEndOfWord
                    && completionPrefix.length() == minAutoCompleteSize //using == so it does not popup after esc and more typing
                    && prefixChanged //ex: pushing esc when the popup is hidden should not raise the popup
            ));

    if (shouldShow)
    {
        c->complete(cr);
    }
    else if (shouldHide)
    {
        c->popup()->hide();
    }
}
