#include "ExportQueryDialog.h"
#include "ui_ExportQueryDialog.h"

ExportQueryDialog::ExportQueryDialog(QStandardItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportQueryDialog),
    m_model(model),
    m_locale(QLocale::system()),
    m_dataFormatter(DataFormatter())
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
    connect(ui->combobox_timeFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_timeFormat, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_timestampFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboboxCurrentIndexChanged(int)));
    connect(ui->combobox_timestampFormat, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->combobox_quote, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboboxCurrentTextChanged(QString)));
    connect(ui->checkbox_includeHeader, &QCheckBox::stateChanged, this, &ExportQueryDialog::on_checkBoxStateChanged);
    connect(ui->checkbox_alwaysQuoteStringColumns, &QCheckBox::stateChanged, this, &ExportQueryDialog::on_checkBoxStateChanged);
    connect(ui->checkbox_replaceNewLine, &QCheckBox::stateChanged, this, &ExportQueryDialog::on_checkBoxStateChanged);

    connect(ui->checkbox_customDateFormat, &QCheckBox::toggled, ui->combobox_dateFormat, &QComboBox::setEnabled);
    connect(ui->checkbox_customTimeFormat, &QCheckBox::toggled, ui->combobox_timeFormat, &QComboBox::setEnabled);
    connect(ui->checkbox_customTimestampFormat, &QCheckBox::toggled, ui->combobox_timestampFormat, &QComboBox::setEnabled);

    connect(ui->checkbox_customDateFormat, &QCheckBox::toggled, this, &ExportQueryDialog::on_checkBoxToggled);
    connect(ui->checkbox_customTimeFormat, &QCheckBox::toggled, this, &ExportQueryDialog::on_checkBoxToggled);
    connect(ui->checkbox_customTimestampFormat, &QCheckBox::toggled, this, &ExportQueryDialog::on_checkBoxToggled);

    ui->textbox_decimalSeparator->setText(QLocale::system().decimalPoint());
    ui->textbox_thousandSeparator->setText(QLocale::system().groupSeparator());

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "settings");
    settings.beginGroup("Export");

    if (settings.contains("dateFormat"))
    {
        ui->checkbox_customDateFormat->setChecked(true);
        ui->combobox_dateFormat->setCurrentText(settings.value("dateFormat").toString());
    }

    if (settings.contains("timeFormat"))
    {
        ui->checkbox_customTimeFormat->setChecked(true);
        ui->combobox_timeFormat->setCurrentText(settings.value("timeFormat").toString());
    }

    if (settings.contains("timestampFormat"))
    {
        ui->checkbox_customTimestampFormat->setChecked(true);
        ui->combobox_timestampFormat->setCurrentText(settings.value("timestampFormat").toString());
    }

    ui->checkbox_includeHeader->setChecked(settings.value("includeHeader", true).toBool());
    ui->checkbox_alwaysQuoteStringColumns->setChecked(settings.value("quoteStringColumns", true).toBool());
    ui->checkbox_replaceNewLine->setChecked(settings.value("replaceNewLine", false).toBool());
    ui->linedit_outputFilePath->setText(settings.value("lastFile", "").toString());
    m_outputFilePath = settings.value("lastFile", "").toString();

    settings.endGroup();

    on_checkBoxToggled(false);
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

    if (ui->checkbox_customDateFormat->isChecked() && !ui->combobox_dateFormat->currentText().isEmpty())
        formatOverrides["dateFormat"] = ui->combobox_dateFormat->currentText();
    if (ui->checkbox_customTimeFormat->isChecked() && !ui->combobox_timeFormat->currentText().isEmpty())
        formatOverrides["timeFormat"] = ui->combobox_timeFormat->currentText();
    if (ui->checkbox_customTimestampFormat->isChecked() && !ui->combobox_timestampFormat->currentText().isEmpty())
        formatOverrides["timestampFormat"] = ui->combobox_timestampFormat->currentText();

    return formatOverrides;
}

QLocale ExportQueryDialog::locale()
{
    return m_locale;
}

bool ExportQueryDialog::includeHeader()
{
    return ui->checkbox_includeHeader->isChecked();
}

bool ExportQueryDialog::alwaysQuoteStrings()
{
    return ui->checkbox_alwaysQuoteStringColumns->isChecked();
}

bool ExportQueryDialog::replaceNewLine()
{
    return ui->checkbox_replaceNewLine->isChecked();
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
    QString delimiter = ui->combobox_delimiter->currentText() == "<tab>" ? "\t" : ui->combobox_delimiter->currentText();
    QString quoteSymbol = ui->combobox_quote->currentText();
    bool alwaysQuoteStrings = ui->checkbox_alwaysQuoteStringColumns->isChecked();
    bool replaceNewLine = ui->checkbox_replaceNewLine->isChecked();
    bool includeHeader = ui->checkbox_includeHeader->isChecked();

    DataFormatter dataFormatter = DataFormatter(formatOverrides());
    Csv csv(delimiter, quoteSymbol, includeHeader, alwaysQuoteStrings, replaceNewLine, dataFormatter);

    ui->textbox_preview->clear();

    ui->textbox_preview->appendPlainText(csv.writeSelectionToString(m_model, 10));
}

void ExportQueryDialog::on_checkBoxStateChanged(int)
{
    refreshText();
}

void ExportQueryDialog::on_checkBoxToggled(bool)
{
    ui->textbox_decimalSeparator->setText(m_locale.decimalPoint());
    ui->textbox_thousandSeparator->setText(m_locale.groupSeparator());

    if (!ui->checkbox_customDateFormat->isChecked())
        ui->combobox_dateFormat->setCurrentText(m_locale.dateFormat(QLocale::ShortFormat));

    if (!ui->checkbox_customTimeFormat->isChecked())
        ui->combobox_timeFormat->setCurrentText(m_locale.timeFormat(QLocale::ShortFormat));

    if (!ui->checkbox_customTimestampFormat->isChecked())
        ui->combobox_timestampFormat->setCurrentText(m_locale.dateTimeFormat(QLocale::ShortFormat));

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

void ExportQueryDialog::on_buttonBox_accepted()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "settings");
    settings.beginGroup("Export");

    if (ui->checkbox_customDateFormat->isChecked() &&  !ui->combobox_dateFormat->currentText().isEmpty())
        settings.setValue("dateFormat", ui->combobox_dateFormat->currentText());
    else
        settings.remove("dateFormat");

    if (ui->checkbox_customTimestampFormat->isChecked() && !ui->combobox_timestampFormat->currentText().isEmpty())
        settings.setValue("timestampFormat", ui->combobox_timestampFormat->currentText());
    else
        settings.remove("timestampFormat");

    if (ui->checkbox_customTimeFormat->isChecked() && !ui->combobox_timeFormat->currentText().isEmpty())
        settings.setValue("timeFormat", ui->combobox_timeFormat->currentText());
    else
        settings.remove("timeFormat");

    settings.setValue("includeHeader", ui->checkbox_includeHeader->isChecked());
    settings.setValue("quoteStringColumns", ui->checkbox_alwaysQuoteStringColumns->isChecked());
    settings.setValue("replaceNewLine", ui->checkbox_replaceNewLine->isChecked());

    settings.setValue("lastFile", ui->linedit_outputFilePath->text());

    settings.endGroup();
}
