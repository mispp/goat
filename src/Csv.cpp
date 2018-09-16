#include "Csv.h"

#include <QDebug>

Csv::Csv(QString delimiter, QString quote, bool includeHeader, bool quoteStringColumns, QLocale locale, QHash<QString, QString> formatOverrides) :
    m_delimiter(delimiter),
    m_quote(quote),
    m_includeHeader(includeHeader),
    m_locale(locale),
    m_quoteStringColumns(quoteStringColumns)
{
    m_dateFormat = formatOverrides.contains("dateFormat") ? formatOverrides["dateFormat"] : locale.dateFormat(QLocale::ShortFormat);
    m_timeFormat = formatOverrides.contains("timeFormat") ? formatOverrides["timeFormat"] : locale.timeFormat(QLocale::ShortFormat);
    m_timestampFormat = formatOverrides.contains("timestampFormat") ? formatOverrides["timestampFormat"] : locale.dateTimeFormat(QLocale::ShortFormat);
}

void Csv::write(QTextStream *stream, QAbstractItemModel *model)
{
    for (int col = 0; col < model->columnCount(); ++col)
    {
        QString value = escape(model->headerData(col, Qt::Horizontal).toString());
        (*stream) << value;

        if (col < model->columnCount() - 1)
            (*stream) << m_delimiter;
    }

    for (int row = 0; row < model->rowCount(); ++row)
    {
        (*stream) << "\n";
        for (int col = 0; col < model->columnCount(); ++col)
        {
            QVariant val = model->index(row, col).data();
            QString value = escape(val.isNull() ? "" : val.toString());
            (*stream) << value;
            if (col < model->columnCount() - 1)
                (*stream) << m_delimiter;
        }
    }
}

void Csv::write(QTextStream *stream, QSqlQuery *query, bool* stopFlag)
{
    if (m_includeHeader)
    {
        QSqlRecord record = query->record();

        QStringList row;

        for (int col=0; col < record.count(); ++col)
        {
            row.append(record.field(col).name().toUpper());
        }

        (*stream) << row.join(m_delimiter);
        (*stream) << endl;
    }

    while (!*stopFlag && query->next())
    {
        QStringList row;

        QSqlRecord record = query->record();

        for (int col=0; col < record.count(); ++col)
        {
            row.append(processValue(record.field(col).value()));
        }

        (*stream) << row.join(m_delimiter);
        (*stream) << endl;
    }
}

QString Csv::writeSelectionToString(QAbstractItemModel *model, const QItemSelection &selection, bool includeHeaders)
{
    QString text;

    QItemSelectionRange range = selection.isEmpty() ? QItemSelectionRange() : selection.first(); //TODO support multiple selections?

    if (includeHeaders)
    {
        int left = range.left();
        int right = range.right();

        if (selection.isEmpty())
        {
            left = 0;
            right = model->columnCount() - 1;
        }

        QStringList rowContents;
        for (auto col = left; col <= right; ++col)
            rowContents << escape(model->headerData(col, Qt::Horizontal).toString());
        text += rowContents.join(m_delimiter);
        text += "\n";
    }

    if (!selection.isEmpty())
    {
        for (auto row = range.top(); row <= range.bottom(); ++row)
        {
            QStringList rowContents;
            for (auto col = range.left(); col <= range.right(); ++col)
            {
                rowContents << processValue(model->index(row, col).data());
            }
            text += rowContents.join(m_delimiter);
            text += "\n";
        }
    }

    return text;
}

QString Csv::writeSelectionToString(QAbstractItemModel *model, bool includeHeaders, int sampleSize)
{
    QString text;

    if (includeHeaders)
    {
        QStringList rowContents;

        for (int col=0; col < model->columnCount(); ++col)
            rowContents << escape(model->headerData(col, Qt::Horizontal).toString(), false);

        text += rowContents.join(m_delimiter);
        text += "\n";
    }

    for (int row=0; row < model->rowCount() && row < sampleSize; ++row)
    {
        QStringList rowContents;
        for (int col=0; col < model->columnCount(); ++col)
        {
            rowContents.append(processValue(model->index(row, col).data()));
        }
        text += rowContents.join(m_delimiter);
        text += "\n";
    }

    return text;
}

QString Csv::escape(QString value, bool quoteStringData)
{
    //TODO replace this with a real csv library
    if (value.contains(m_delimiter) || value.contains("\n") || value.contains(m_quote))
        return m_quote + value.replace(m_quote, m_quote + m_quote) + m_quote;
    else if (quoteStringData)
        return m_quote + value + m_quote;
    else
        return value;
}

QString Csv::processValue(QVariant value)
{
    QString processedValue;

    if (value.isNull())
        processedValue = "";
    else if (value.type() == QVariant::Date)
        processedValue = value.toDate().toString(m_dateFormat);
    else if (value.type() == QVariant::DateTime)
        processedValue = value.toDateTime().toString(m_timestampFormat);
    else if (value.type() == QVariant::Time)
        processedValue = value.toTime().toString(m_timeFormat);
    else if (value.type() == QVariant::String)
        processedValue = escape(value.toString(), m_quoteStringColumns);
    else if (value.type() == QVariant::Double)
        processedValue = m_locale.toString(value.toDouble());
    else
        processedValue = value.toString();

    return processedValue;
}
