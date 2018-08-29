#include "StringUtils.h"

QString StringUtils::interpolate(QString string, QMap<QString, QString> values)
{
    foreach (QString key, values.keys())
    {
        string = string.replace("%(" + key + ")", values[key]);
    }
    return string;
}
