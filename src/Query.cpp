#include "Query.h"

Query::Query(QObject *parent) :
    QObject(parent),
    m_queryConnecionId("CLONED_" + QUuid::createUuid().toString()),
    m_isFinished(true)
{

}

Query::~Query()
{
    m_query.finish();

    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }
}

bool Query::isFinished()
{
    return m_isFinished;
}

bool Query::isSelect()
{
    return m_query.isSelect();
}

Connection Query::connection()
{
    return m_connection;
}

int Query::sessionPid()
{
    return m_sessionPid;
}

QString Query::lastQuery()
{
    return m_query.lastQuery();
}

void Query::executeSql(QString sql, Connection connection)
{
    m_connection = connection;

    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }

    //QSqlDatabase clonedConnection = ConnectionManager::cloneConnection(connection, m_queryConnecionId);

    QSqlDatabase clonedDatabase = QSqlDatabase::addDatabase(connection.driver(), m_queryConnecionId);

    clonedDatabase.setHostName(connection.details()["server"]);
    clonedDatabase.setPort(connection.details()["port"].toInt());
    clonedDatabase.setDatabaseName(connection.details()["database"]);
    clonedDatabase.setConnectOptions(connection.details()["options"]);
    clonedDatabase.setUserName(connection.details()["username"]);
    clonedDatabase.setPassword(connection.details()["pass"]);

    bool ok = clonedDatabase.open();

    if (!ok)
    {
        QSqlDatabase::removeDatabase(connection.connectionId());

        QStringList errorList;
        errorList.append(clonedDatabase.lastError().driverText());
        errorList.append(clonedDatabase.lastError().databaseText());
        errorList.append(clonedDatabase.lastError().nativeErrorCode());

        emit queryExecutionFailed(errorList);
    }
    else
    {
        m_sessionPid = getSessionPid(clonedDatabase);

        m_isFinished = false;

        m_query = QSqlQuery(clonedDatabase);
        m_query.setForwardOnly(true);

        bool queryOK = m_query.exec(sql);

        if (queryOK)
        {
            QStringList message;

            emit queryExecutionFinished(m_query.isSelect(), m_query.record(), message);
        }
        else
        {
            QStringList message;
            message.append(m_query.lastError().text());

            emit queryExecutionFailed(message);
        }

        m_isFinished = true;
    }

    m_sessionPid = -1;
}

void Query::requestNextRowSet(int rowCount)
{
    RowSet rows;

    int counter = 0;

    while (counter < rowCount && m_query.next())
    {
        QSqlRecord record = m_query.record();

        Row row;

        for (int col = 0; col < record.count(); ++col)
        {
            QStandardItem* item = new QStandardItem();
            item->setData(record.value(col), Qt::DisplayRole);
            if (record.value(col).isNull())
            {
                item->setData(QColor("#f7ff87"), Qt::BackgroundRole);
            }
            row.append(item);
        }

        rows.append(row);

        ++counter;
    }

    emit nextRowSet(rows);
}

int Query::getSessionPid(QSqlDatabase clonedDatabase)
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
