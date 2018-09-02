#include <QApplication>
#include <QtTest>

#include "TestStringUtils.h"

//following pattern from: http://xilexio.org/?p=125
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    TestStringUtils testStringUtils;

    return 0
            | QTest::qExec(&testStringUtils, argc, argv);
}
