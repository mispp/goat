#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QList>
#include <QStandardItemModel>
#include <QMutex>

#include "ConnectionStandardItem.h"

class ConnectionManager
{
public:
    QStandardItemModel* getModel();
    static ConnectionManager* getInstance();

    void establishConnection(ConnectionStandardItem* connection);
    int connectionsAvailable();
    QStringList getEstablishedConnectionList();
    QStandardItemModel* getEstablishedConnectionModel();

private:
	ConnectionManager();

	static ConnectionManager* m_instance;
    QStandardItemModel* m_establishedConnectionModel;
};

#endif // CONNECTIONMANAGER_H
