#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QList>
#include "Connection.h"
#include <QStandardItemModel>
#include <QMutex>

class ConnectionManager
{
public:
	void addConnection(Connection* connection);
	bool connectionAvailable(Connection* connection);
	QStandardItemModel* getModel();
	static ConnectionManager* getInstance();
	Connection* getDefaultConnection();

private:
	ConnectionManager();

	QList<Connection*> m_connectionsList;
	QStandardItemModel* m_model;
	void updateModel(Connection* connection);

	static ConnectionManager* m_instance;
	static QMutex m_mutex;

};

#endif // CONNECTIONMANAGER_H
