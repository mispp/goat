#ifndef CONNECTION_H
#define CONNECTION_H

#include <QString>
#include <QVariant>
#include <QSettings>

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
	QString getConnectionId();

    void setDriver(QString newDriver);
    void setUser(QString newUser);
    void setPass(QString newPass);
    void setServer(QString newServer);
    void setPort(int newPort);
    void setDatabase(QString newDatabase);
    void setName(QString newName);

    void persistConnection();

private:
    QString m_name;
    QString m_driver;
    QString m_user;
    QString m_pass;
    QString m_server;
    int m_port;
    QString m_database;
    QString m_connectionId;
};

Q_DECLARE_METATYPE(Connection*)

#endif // CONNECTION_H
