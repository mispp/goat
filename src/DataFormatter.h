#ifndef DATAFORMATTER_H
#define DATAFORMATTER_H

#include <QLocale>
#include <QVariant>
#include <QDate>
#include <QDateTime>
#include <QDebug>

class DataFormatter
{
public:
    DataFormatter(QString delimiter = ";", QString quoteSymbol = "\"", bool alwaysQuoteStrings = true, bool replaceNewLine = true, QHash<QString, QString> formatOverrides = QHash<QString, QString>());
    ~DataFormatter();

    QString format(QVariant value);

private:
    QString m_delimiter;
    QString m_quoteSymbol;
    bool m_alwaysQuoteStrings;
    bool m_replaceNewLine;
    QHash<QString, QString> m_formatOverrides;

    QString escapeAndQuote(const QString value);
    QString dateFormat();
    QString timeFormat();
    QString timestampFormat();

};

#endif // DATAFORMATTER_H
