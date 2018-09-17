#ifndef CSV_H
#define CSV_H

#include "src/DataFormatter.h"

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
    Csv(QString delimiter = ";", bool includeHeader = true, DataFormatter dataFormatter = DataFormatter());
    void write(QTextStream *stream, QAbstractItemModel *model);
    void write(QTextStream *stream, QSqlQuery *query, bool* stopFlag);
    QString writeSelectionToString(QAbstractItemModel *model, const QItemSelection &selection, bool includeHeaders = true);
    QString writeSelectionToString(QAbstractItemModel *model, const int sampleSize = 10);
private:
    QString m_delimiter;
    bool m_includeHeader;
    DataFormatter m_dataFormatter;

};

#endif // CSV_H
