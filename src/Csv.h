#ifndef CSV_H
#define CSV_H

#include "../src/DataFormatter.h"

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QString>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QDate>
#include <QDateTime>
#include <QTime>

class Csv
{

public:
    Csv(QString delimiter = ";", QString quoteSymbol = "\"", bool includeHeader = true, bool alwaysQuoteStrings = true, bool replaceNewLineSymbolWithText = false, DataFormatter dataFormatter = DataFormatter());
    void write(QTextStream *stream, QAbstractItemModel *model);
    void write(QTextStream *stream, QSqlQuery *query, bool* stopFlag);
    QString writeSelectionToString(QAbstractItemModel *model, const QItemSelection &selection, bool includeHeaders = true);
    QString writeSelectionToString(QAbstractItemModel *model, const int sampleSize = 10);

private:
    QString m_delimiter;
    QString m_quoteSymbol;
    bool m_includeHeader;
    bool m_alwaysQuoteStrings;
    bool m_replaceNewLineSymbolWithText;
    DataFormatter m_dataFormatter;

    QString escapeAndQuote(const QString value);

};

#endif // CSV_H
