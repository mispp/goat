#ifndef CSV_H
#define CSV_H

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
    Csv(QString delimiter = ",", QString quote = "\"", bool includeHeader = true, bool quoteStringColumns = true, QLocale locale = QLocale::system(), QHash<QString, QString> formatOverrides = QHash<QString, QString>());
    void write(QTextStream *stream, QAbstractItemModel *model);
    void write(QTextStream *stream, QSqlQuery *query, bool* stopFlag);
    QString writeSelectionToString(QAbstractItemModel *model, const QItemSelection &selection, bool includeHeaders = true);
    QString writeSelectionToString(QAbstractItemModel *model, const bool includeHeaders, const int sampleSize = 10);
private:
    QString m_delimiter;
    QString m_quote;
    bool m_includeHeader;
    QLocale m_locale;
    bool m_quoteStringColumns;

    QString m_timeFormat;
    QString m_timestampFormat;
    QString m_dateFormat;

    QString escape(QString value, bool quoteStringData = true);
    QString processValue(QVariant value);
};

#endif // CSV_H
