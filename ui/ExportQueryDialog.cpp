#include "ExportQueryDialog.h"
#include "ui_ExportQueryDialog.h"

ExportQueryDialog::ExportQueryDialog(QStandardItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportQueryDialog),
    m_model(model)
{
    ui->setupUi(this);
    ui->textbox_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    connect(ui->linedit_outputFilePath, &QLineEdit::textChanged, this, &ExportQueryDialog::checkFilename);

    connect(ui->combobox_delimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int))); //weird, doesnt work with new signal/slot syntax
    connect(ui->combobox_delimiter, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_quote, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_quote, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_dateFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_dateFormat, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_locale, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_locale, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_timeFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_timeFormat, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_timestampFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_timestampFormat, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_quote, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->checkbox_includeHeader, &QCheckBox::stateChanged, this, &ExportQueryDialog::on_checkBoxStateChanged);

    QMap<QString, QLocale> allLocales;

    foreach (QLocale locale, QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry))
    {
        allLocales[QLocale::countryToString(locale.country()) + " (" + locale.name() + ")"] = locale;
    }

    ui->combobox_locale->addItem("<system locale>", QLocale::system());

    foreach(QString key, allLocales.keys())
    {
        ui->combobox_locale->addItem(key, allLocales[key]);
    }

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

QHash<QString, QString> ExportQueryDialog::formatOverrides()
{
    QHash<QString, QString> formatOverrides;

    if (ui->checkbox_customDateFormat->isChecked())
        formatOverrides["dateformat"] = ui->combobox_dateFormat->currentText();
    if (ui->checkbox_customTimeFormat->isChecked())
        formatOverrides["timeformat"] = ui->combobox_timeFormat->currentText();
    if (ui->checkbox_customTimestampFormat->isChecked())
        formatOverrides["timestampformat"] = ui->combobox_timestampFormat->currentText();

    return formatOverrides;
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

    ui->textbox_preview->clear();

    Csv csv(delimiter, quoteSymbol);

    //QString numberFormat = ui->combobox_numberFormat->currentText();
    QString dateFormat = ui->combobox_dateFormat->currentText();
    QString timestampFormat = ui->combobox_timestampFormat->currentText();
    QString timeFormat = ui->combobox_timeFormat->currentText();

    //numberFormat = "%+06.2f";

    ui->textbox_preview->appendPlainText(csv.writeSelectionToString(m_model, ui->checkbox_includeHeader->isChecked(), 10));
}

void ExportQueryDialog::on_checkBoxStateChanged(int)
{
    refreshText();
}

void ExportQueryDialog::on_comboboxCurrentTextChanged(QString)
{
    refreshText();
}

void ExportQueryDialog::on_comboboxCurrentIndexChanged(int)
{
    refreshText();
}

void ExportQueryDialog::on_combobox_locale_currentIndexChanged(int index)
{
    QLocale selectedLocale = ui->combobox_locale->itemData(index).value<QLocale>();

    ui->textbox_decimalSeparator->setText(selectedLocale.decimalPoint());
    ui->textbox_thousandSeparator->setText(selectedLocale.groupSeparator());

    ui->combobox_dateFormat->removeItem(0);
    ui->combobox_dateFormat->insertItem(0, selectedLocale.dateFormat(QLocale::ShortFormat));
    ui->combobox_dateFormat->setCurrentIndex(0);

    ui->combobox_timestampFormat->removeItem(0);
    ui->combobox_timestampFormat->insertItem(0, selectedLocale.dateTimeFormat(QLocale::ShortFormat));
    ui->combobox_timestampFormat->setCurrentIndex(0);

    ui->combobox_timeFormat->removeItem(0);
    ui->combobox_timeFormat->insertItem(0, selectedLocale.timeFormat(QLocale::ShortFormat));
    ui->combobox_timeFormat->setCurrentIndex(0);
}
