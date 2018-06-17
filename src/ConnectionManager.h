#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QList>
#include "Connection.h"
#include <QStandardItemModel>
#include <QMutex>

class ConnectionManager
{
public:
	Connection* getDefaultConnection();
    QStandardItemModel* getModel();

    static ConnectionManager* getInstance();
    void establishConnection(Connection* connection);
    void loadSavedConnections();
    int connectionsAvailable();


private:
	ConnectionManager();

    QList<Connection*> m_availableConnectionList;
    QList<Connection*> m_establishedConnectionsList;

    QStandardItemModel* m_connectionListModel;

    void updateModel();
	static ConnectionManager* m_instance;
};

#endif // CONNECTIONMANAGER_H
