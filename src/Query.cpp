#include "Query.h"

Query::Query(ConnectionManager* connectionManager, QObject *parent) :
    AbstractQuery(connectionManager, parent)
{

}

Query::~Query()
{

}

void Query::executeSql(QString sql, Connection connection)
{
    m_connection = Connection(m_queryConnecionId, connection.driver(), "CLONED_" + connection.name(), connection.details());
    m_connectionManager->closeConnection(m_queryConnecionId);
    m_connectionManager->openConnection(m_connection);

    QSqlDatabase clonedDatabase = m_connectionManager->getOpenConnection(m_queryConnecionId);

    bool ok = clonedDatabase.open();

    if (!ok)
    {
        QStringList errorList;
        errorList.append(clonedDatabase.lastError().driverText());
        errorList.append(clonedDatabase.lastError().databaseText());
        errorList.append(clonedDatabase.lastError().nativeErrorCode());

        m_connectionManager->closeConnection(m_queryConnecionId);

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

    m_isFinished = true;
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

