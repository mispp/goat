#ifndef EXPORTQUERYDIALOG_H
#define EXPORTQUERYDIALOG_H

#include "src/Csv.h"

#include <QDebug>
#include <QAbstractButton>
#include <QDialog>
#include <QFileDialog>
#include <QFontDatabase>
#include <QStandardItemModel>
#include <QSettings>

namespace Ui {
class ExportQueryDialog;
}

class ExportQueryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportQueryDialog(QStandardItemModel* model, QWidget *parent = 0);
    ~ExportQueryDialog();

    QString outputFilePath();
    QString delimiter();
    QString quoteSymbol();
    QLocale locale();
    QHash<QString, QString> formatOverrides();
    bool includeHeader();
    bool quoteStringColumns();

public slots:
    void checkFilename(const QString text);
    void on_checkBoxStateChanged(int);
    void on_checkBoxToggled(bool);
    void on_comboboxCurrentIndexChanged(int);
    void on_comboboxCurrentTextChanged(QString);

private slots:
    void on_pushButton_released();
    void on_linedit_outputFilePath_textChanged(const QString &arg1);
    void on_buttonBox_accepted();

private:
    Ui::ExportQueryDialog *ui;
    QString m_outputFilePath;

    QStandardItemModel* m_model;

    QLocale m_locale;

    void refreshText();
};

#endif // EXPORTQUERYDIALOG_H
