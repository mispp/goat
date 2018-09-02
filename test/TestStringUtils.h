#ifndef TESTSTRINGUTILS_H
#define TESTSTRINGUTILS_H

#include <QtTest>

class TestStringUtils : public QObject
{
    Q_OBJECT

  private slots:

    void testInterpolate();
};

#endif // TESTSTRINGUTILS_H
