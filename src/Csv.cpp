#include "Csv.h"

#include <QDebug>

Csv::Csv(QString delimiter, QString quote)
{
    m_delimiter = delimiter;
    m_quote = quote;
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
    QSqlRecord header = query->record();

    while (!*stopFlag && query->next())
    {
        QStringList row;

        QSqlRecord record = query->record();

        for (int col=0; col < record.count(); ++col)
        {
            QSqlField field = record.field(col);
            QString value = escape(field.value().toString());

            row.append(value);
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
                QVariant value = model->index(row, col).data();
                rowContents << escape(value.isNull() ? "" : value.toString());
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
            rowContents << escape(model->headerData(col, Qt::Horizontal).toString());
        text += rowContents.join(m_delimiter);
        text += "\n";
    }

    for (int row=0; row < model->rowCount() && row < sampleSize; ++row)
    {
        QStringList rowContents;
        for (int col=0; col < model->columnCount(); ++col)
        {
            QVariant value = model->index(row, col).data();
            rowContents << escape(value.isNull() ? "" : value.toString());
        }
        text += rowContents.join(m_delimiter);
        text += "\n";
    }

    return text;
}

QString Csv::escape(QString value)
{
    //TODO replace this with a real csv library
    if (value.contains(m_delimiter) || value.contains("\n") || value.contains(m_quote))
        return m_quote + value.replace(m_quote, m_quote + m_quote) + m_quote;
    return value;
}
