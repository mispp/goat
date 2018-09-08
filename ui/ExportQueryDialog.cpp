#include "ExportQueryDialog.h"
#include "ui_ExportQueryDialog.h"

ExportQueryDialog::ExportQueryDialog(QStandardItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportQueryDialog),
    m_model(model)
{
    ui->setupUi(this);
    ui->textedit_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    connect(ui->linedit_outputFilePath, &QLineEdit::textChanged, this, &ExportQueryDialog::checkFilename);

    connect(ui->combobox_delimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int))); //weird, doesnt work with new signal/slot syntax
    connect(ui->combobox_quote, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->checkbox_includeHeader, &QCheckBox::stateChanged, this, &ExportQueryDialog::on_checkBoxStateChanged);

    refreshText();
}

ExportQueryDialog::~ExportQueryDialog()
{
    delete ui;
}

void ExportQueryDialog::on_pushButton_released()
{
    m_outputFilePath = QFileDialog::getSaveFileName(this, "Save file", QDir::homePath(), ".csv files (*.csv)");
    ui->linedit_outputFilePath->setText(m_outputFilePath);
}

QString ExportQueryDialog::outputFilePath()
{
    return m_outputFilePath;
}

QString ExportQueryDialog::delimiter()
{
    return ui->combobox_delimiter->currentText();
}

QString ExportQueryDialog::quoteSymbol()
{
    return ui->combobox_quote->currentText();
}

void ExportQueryDialog::on_linedit_outputFilePath_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty())
    {
        ui->label_error->clear();
    }
}

void ExportQueryDialog::checkFilename(const QString text)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(text.isEmpty());
}

void ExportQueryDialog::refreshText()
{
    QString delimiter = ui->combobox_delimiter->currentText();
    QString quoteSymbol = ui->combobox_delimiter->currentText();

    ui->textedit_preview->clear();

    Csv csv(delimiter, quoteSymbol);

    ui->textedit_preview->appendPlainText(csv.writeSelectionToString(m_model, ui->checkbox_includeHeader->isChecked(), 10));
}

void ExportQueryDialog::on_checkBoxStateChanged(int)
{
    refreshText();
}

void ExportQueryDialog::on_comboboxCurrentIndexChanged(int)
{
    refreshText();
}
