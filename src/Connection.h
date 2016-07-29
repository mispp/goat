#ifndef CONNECTION_H
#define CONNECTION_H

#include <QString>
#include <QVariant>
#include <QListWidgetItem>



class Connection
{
public:
	Connection(QString p_Driver, QString p_User, QString p_Pass, QString p_Server, int p_Port, QString p_Database, QString p_Name, QString p_GroupName);
	QString getDriver();
	QString getUser();
	QString getPass();
	QString getServer();
	int getPort();
	QString getDatabase();
	QString getName();
	void setChanged();
	QString getConnectionId();

private:
	QString m_Name;
	QString m_Driver;
	QString m_User;
	QString m_Pass;
	QString m_Server;
	int m_Port;
	QString m_Database;
	QString m_GroupName;
	QString m_connectionId;

	bool m_changed;
};

Q_DECLARE_METATYPE(Connection*)

#endif // CONNECTION_H
