#ifndef EXPORTQUERYDIALOG_H
#define EXPORTQUERYDIALOG_H

#include "src/Csv.h"

#include <QAbstractButton>
#include <QDialog>
#include <QFileDialog>
#include <QFontDatabase>
#include <QStandardItemModel>

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

public slots:
    void checkFilename(const QString text);
    void on_checkBoxStateChanged(int);
    void on_comboboxCurrentIndexChanged(int);
    void on_comboboxCurrentTextChanged(QString);

private slots:
    void on_pushButton_released();
    void on_linedit_outputFilePath_textChanged(const QString &arg1);

    void on_combobox_locale_currentIndexChanged(int index);

private:
    Ui::ExportQueryDialog *ui;
    QString m_outputFilePath;

    QStandardItemModel* m_model;

    void refreshText();
};

#endif // EXPORTQUERYDIALOG_H
