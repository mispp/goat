#include "Connection.h"

#include <QUuid>
#include <assert.h>

Connection::Connection()
{
    m_connectionId = QUuid::createUuid().toString().mid(1, 36);
    m_name = "New connection";
    m_driver = "QPSQL";

    m_details["server"] = "";
    m_details["port"] = "";
    m_details["database"] = "";
    m_details["username"] = "";
    m_details["pass"] = "";
}

Connection::Connection(const QString &connectionId, const QString &driver, const QString &name, const QMap<QString, QString> &details)
{
    assert(!connectionId.isEmpty());
    assert(!driver.isEmpty());

    m_connectionId = connectionId;
    m_name = name;
    m_driver = driver;
    m_details = details;
}

QMap<QString, QString> Connection::details() const
{
    return m_details;
}

void Connection::setDetails(const QMap<QString, QString> &details)
{
    m_details = details;
}

void Connection::setDriver(const QString &driver)
{
    m_driver = driver;
}

QString Connection::driver() const
{
    return m_driver;
}

QString Connection::name() const
{
    return m_name;
}

void Connection::setName(const QString &name)
{
    m_name = name;
}

QString Connection::connectionId() const
{
    return m_connectionId;
}
