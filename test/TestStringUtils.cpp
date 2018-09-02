#include "TestStringUtils.h"
#include "src/StringUtils.h"

void TestStringUtils::testInterpolate()
{
    QString in = "Hello %(name)!";
    QMap<QString, QString> values;
    values["name"] = "World";

    QCOMPARE(StringUtils::interpolate(in, values), QString("Hello World!"));
}
