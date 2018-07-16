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
    void saveConnection(const Connection &connection);
    void deleteConnection(const QString &connectionId);
    void openConnection(const Connection &connection);
    void closeConnection(const QString &connectionId);
    QMap<QString, Connection> getConnections() const;
    QMap<QString, QSqlDatabase> getOpenConnections() const;

private:
    QList<Connection> loadConnections();

    QMap<QString, Connection> m_connections;
    QMap<QString, QSqlDatabase> m_openConnections;
};

#endif // CONNECTIONMANAGER_H
