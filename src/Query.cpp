#include "Query.h"

#include <QVariant>

Query::Query(ConnectionManager* connectionManager, QObject *parent) :
    AbstractQuery(connectionManager, parent)
{
    m_numberTypeIds << 2 << 3 << 6 << 32 << 4 << 35 << 5 << 36 << 38;
    m_stringTypeIds << 7 << 10 << 34;
    m_timeTypeIds << 14 << 15 << 16;
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

        QDateTime start = QDateTime::currentDateTime();

        m_query = QSqlQuery(clonedDatabase);
        m_query.setForwardOnly(true);

        bool queryOK = m_query.exec(sql);

        QDateTime end = QDateTime::currentDateTime();

        QStringList message;
        message.append("Timestamp: " + start.toString("yyyy-MM-dd hh:mm:ss"));
        message.append("Elapsed: ");
        message.append("  ms: " + QString::number(start.msecsTo(end)));
        message.append("   s: " + QString::number(start.secsTo(end)));
        message.append("------------------------------");

        if (queryOK)
        {
            if (!m_query.isSelect())
            {
                message.append("Number of rows affected: " + QString::number(m_query.numRowsAffected()));
                message.append("------------------------------");
            }
            message.append(m_query.lastQuery());

            emit finished(m_query.isSelect(), m_query.record(), message);
        }
        else
        {
            message.append(m_query.lastError().text());
            message.append("------------------------------");
            message.append(m_query.lastQuery());

            emit failed(message);
        }
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

            int type_id  = record.field(col).type();

            if (m_numberTypeIds.contains(type_id))
                item->setData(QColor("#1746d3"), Qt::ForegroundRole); //numers -> blue
            else if (m_stringTypeIds.contains(type_id))
                item->setData(QColor("#ba2837"), Qt::ForegroundRole); //string -> dark red
            else if (m_timeTypeIds.contains(type_id))
                item->setData(QColor("#0fc643"), Qt::ForegroundRole); //time/date -> green

            row.append(item);
        }

        rows.append(row);

        ++counter;
    }

    emit nextRowSet(rows);
}

