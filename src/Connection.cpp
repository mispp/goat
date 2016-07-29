#include "Connection.h"
#include "StaticFunctions.h"

Connection::Connection(QString p_Driver, QString p_User, QString p_Pass, QString p_Server, int p_Port, QString p_Database, QString p_Name, QString p_GroupName)
{
	m_Name = p_Name;
	m_Driver = p_Driver;
	m_User = p_User;
	m_Pass = p_Pass;
	m_Server = p_Server;
	m_Port = p_Port;
	m_Database = p_Database;
	m_GroupName = p_GroupName;

	m_connectionId = StaticFunctions::getRandomString();

}

QString Connection::getDriver()
{
	return m_Driver;
}

QString Connection::getUser()
{
	return m_User;
}

QString Connection::getPass()
{
	return m_Pass;
}

QString Connection::getServer()
{
	return m_Server;
}

int Connection::getPort()
{
	return m_Port;
}

QString Connection::getDatabase()
{
	return m_Database;
}

QString Connection::getName()
{
	//return m_Name;
	return m_User + "@" + m_Server;
}

void Connection::setChanged()
{
	m_changed = true;
}

QString Connection::getConnectionId()
{
	return m_connectionId;
}
