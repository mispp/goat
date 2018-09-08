#include "AbstractQuery.h"

AbstractQuery::AbstractQuery(ConnectionManager* connectionManager, QObject *parent) :
    QObject(parent),
    m_connectionManager(connectionManager),
    m_queryConnecionId("CLONED_" + QUuid::createUuid().toString()),
    m_isFinished(true)
{

}

AbstractQuery::~AbstractQuery()
{
    m_query.finish();

    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }
}

bool AbstractQuery::isFinished()
{
    return m_isFinished;
}

bool AbstractQuery::isSelect()
{
    return m_query.isSelect();
}

Connection AbstractQuery::connection()
{
    return m_connection;
}

int AbstractQuery::sessionPid()
{
    return m_sessionPid;
}

QString AbstractQuery::lastQuery()
{
    return m_query.lastQuery();
}

int AbstractQuery::getSessionPid(QSqlDatabase clonedDatabase)
{
    QString sql;

    if (clonedDatabase.driverName() == "QPSQL")
        sql = "select pg_backend_pid();";
    else if (clonedDatabase.driverName() == "QMYSQL")
        sql = "SELECT CONNECTION_ID();";
    else if (clonedDatabase.driverName() == "QODBC")
        sql = "SELECT @@SPID;";

    QSqlQuery q(clonedDatabase);
    q.prepare(sql);
    bool result = q.exec();
    if (result)
    {
        q.next();
        int pid = q.value(0).toInt();

        if (pid > 0) return pid;
        else return -1;
    }
    else return -1;
}

