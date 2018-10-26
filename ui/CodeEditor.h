#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QString>

#include "ui/Highlighter.h"

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    QString getQueryAtCursor();
    QString getSelection();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    void highlightCurrentLine();
    QString textUnderCursor() const;
    Highlighter m_highlighter;

    QRegularExpression multilineCommentExpression;
    QRegularExpression singlelineCommentExpression;
    QStringListModel *m_completerModel;
    QCompleter m_completer;

};

#endif // CODEEDITOR_H
