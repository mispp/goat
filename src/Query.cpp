#include "Query.h"

Query::Query(QObject *parent) : AbstractQuery(parent)
{

}

Query::~Query()
{

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

        emit failed(errorList);
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

            emit finished(m_query.isSelect(), m_query.record(), message);
        }
        else
        {
            QStringList message;
            message.append(m_query.lastError().text());

            emit failed(message);
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

