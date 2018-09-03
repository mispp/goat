#include "Query.h"

Query::Query(QObject *parent) :
    QObject(parent),
    m_queryConnecionId("CLONED_" + QUuid::createUuid().toString()),
    m_stopFlag(false)
{
    qDebug() << "query created with id: " + m_queryConnecionId;
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

void Query::executeSql(QString sql, Connection connection)
{
    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }

    //QSqlDatabase clonedConnection = ConnectionManager::cloneConnection(connection, m_queryConnecionId);

    QSqlDatabase clonedConnection = QSqlDatabase::addDatabase(connection.driver(), m_queryConnecionId);

    clonedConnection.setHostName(connection.details()["server"]);
    clonedConnection.setPort(connection.details()["port"].toInt());
    clonedConnection.setDatabaseName(connection.details()["database"]);
    clonedConnection.setConnectOptions(connection.details()["options"]);
    clonedConnection.setUserName(connection.details()["username"]);
    clonedConnection.setPassword(connection.details()["pass"]);

    bool ok = clonedConnection.open();

    if (!ok)
    {
        QSqlDatabase::removeDatabase(connection.connectionId());

        QStringList errorList;
        errorList.append(clonedConnection.lastError().driverText());
        errorList.append(clonedConnection.lastError().databaseText());
        errorList.append(clonedConnection.lastError().nativeErrorCode());

        emit queryExecutionFailed(errorList);
    }
    else
    {
        m_query = QSqlQuery(clonedConnection);
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
    }
}

void Query::executeSqlAndExport(QString sql, Connection connection, QString outputFilePath, Csv csvHandler)
{

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

void Query::stopExport()
{

}
