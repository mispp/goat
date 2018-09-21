#include "DataFormatter.h"

DataFormatter::DataFormatter(QHash<QString, QString> formatOverrides) :
    m_formatOverrides(formatOverrides)
{

}

DataFormatter::~DataFormatter()
{

}

QString DataFormatter::format(QVariant value)
{
    QString processedValue;

    if (value.isNull())
        processedValue =  "";
    else if (value.type() == QVariant::Date)
        processedValue = value.toDate().toString(dateFormat());
    else if (value.type() == QVariant::DateTime)
        processedValue = value.toDateTime().toString(timestampFormat());
    else if (value.type() == QVariant::Time)
        processedValue = value.toTime().toString(timeFormat());
    else if (value.type() == QVariant::Double)
        processedValue = QLocale::system().toString(value.toDouble());
    else
        processedValue = value.toString();

    return processedValue;
}


QString DataFormatter::dateFormat()
{
    return m_formatOverrides.contains("dateFormat") ? m_formatOverrides["dateFormat"] : QLocale::system().dateFormat(QLocale::ShortFormat);
}

QString DataFormatter::timeFormat()
{
    return m_formatOverrides.contains("timeFormat") ? m_formatOverrides["timeFormat"] : QLocale::system().timeFormat(QLocale::ShortFormat);
}

QString DataFormatter::timestampFormat()
{
    return m_formatOverrides.contains("timestampFormat") ? m_formatOverrides["timestampFormat"] : QLocale::system().dateTimeFormat(QLocale::ShortFormat);
}
