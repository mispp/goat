#include "Connection.h"

Connection::Connection(QString p_Driver, QString p_User, QString p_Pass, QString p_Server, int p_Port, QString p_Database, QString p_Name, QString p_ConnectionId)
{
    m_name = p_Name;
    m_driver = p_Driver;
    m_user = p_User;
    m_pass = p_Pass;
    m_server = p_Server;
    m_port = p_Port;
    m_database = p_Database;
    m_connectionId = p_ConnectionId;
}

QString Connection::getDriver()
{
    return m_driver;
}

void Connection::setDriver(QString newDriver)
{
    m_driver = newDriver;
}

QString Connection::getUser()
{
    return m_user;
}

void Connection::setUser(QString newUser)
{
    m_user = newUser;
}

QString Connection::getPass()
{
    return m_pass;
}

void Connection::setPass(QString newPass)
{
    m_pass = newPass;
}

QString Connection::getServer()
{
    return m_server;
}

void Connection::setServer(QString newServer)
{
    m_server = newServer;
}

int Connection::getPort()
{
    return m_port;
}

void Connection::setPort(int newPort)
{
    m_port = newPort;
}

QString Connection::getDatabase()
{
    return m_database;
}

void Connection::setDatabase(QString newDatabase)
{
    m_database = newDatabase;
}

QString Connection::getName()
{
    return m_name;
}

void Connection::setName(QString newName)
{
    m_name = newName;
}

QString Connection::getConnectionId()
{
	return m_connectionId;
}

void Connection::persistConnection()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    settings.beginGroup(m_connectionId);

    settings.setValue("name", m_name);
    settings.setValue("driver", m_driver);
    settings.setValue("server", m_server);
    settings.setValue("port", m_port);
    settings.setValue("database", m_database);
    settings.setValue("username", m_user);
    settings.setValue("pass", m_pass);

    settings.endGroup();
}
