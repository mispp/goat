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
    static QList<Connection*> loadSavedConnections();

    void establishConnection(Connection* connection);
    int connectionsAvailable();

    //static bool createConnection(Connection* connection);
    //static bool updateConnection(Connection* connection);

    static QMap<QString, QStringList> getSavedConnections();
    QStringList getEstablishedConnectionList();

private:
	ConnectionManager();

    QList<Connection*> m_availableConnectionList;

    QStandardItemModel* m_connectionListModel;

    QString m_lastUsedConnection;

    void updateModel();
	static ConnectionManager* m_instance;
};

#endif // CONNECTIONMANAGER_H
