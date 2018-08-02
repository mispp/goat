#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QList>
#include <QMap>
#include <QSqlDatabase>
#include<QStandardItemModel>

#include "src/Connection.h"

class ConnectionManager
{
public:
    ConnectionManager();
    ~ConnectionManager();
    void saveConnection(const Connection &connection);
    void deleteConnection(const QString &connectionId);
    void openConnection(const Connection &connection);
    void closeConnection(const QString &connectionId);
    QMap<QString, Connection> getConnections() const;
    bool isOpen(const QString &connectionId) const;
    QSqlDatabase getOpenConnection(const QString &connectionId);

private:
    QList<Connection> loadConnections();

    QMap<QString, Connection> m_connections;
};

#endif // CONNECTIONMANAGER_H
