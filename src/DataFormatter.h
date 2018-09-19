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
    DataFormatter(QHash<QString, QString> formatOverrides = QHash<QString, QString>());
    ~DataFormatter();

    QString format(QVariant value);

private:
    QHash<QString, QString> m_formatOverrides;

    QString dateFormat();
    QString timeFormat();
    QString timestampFormat();

};

#endif // DATAFORMATTER_H
