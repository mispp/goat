#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QMap>
#include <QString>

class StringUtils
{

public:

    /*!
        \brief replaces named placeholders with values.

        ex: string="hello there %(name)!", values = {"name": "Matt"} returns "hello there Matt!"
    */
    static QString interpolate(QString string, QMap<QString, QString> values);
};

#endif // STRINGUTILS_H
