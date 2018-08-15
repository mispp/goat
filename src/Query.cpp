#include "Query.h"

#include <QtConcurrent/QtConcurrent>
#include <QSqlRecord>
#include <QSqlError>

Query::Query() : QObject()
{

}

Query::~Query()
{

}

const QList<TableRow> Query::getNextRowSet(int rowSetSize)
{
    QList<TableRow> rows;

    int counter = 0;

    while (counter < rowSetSize && m_query.next())
    {
        QSqlRecord record = m_query.record();

        TableRow row;

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

    return rows;
}

void Query::switchDatabase(QString connectionId)
{
    if (m_database.isOpen())
        m_database.close();

    QSqlDatabase::removeDatabase(m_database.connectionName());

    m_database = QSqlDatabase::cloneDatabase(QSqlDatabase::database(connectionId), "CLONED_" + QUuid::createUuid().toString());
    m_database.open();

    m_postgresBackendPID = -1;
}

void Query::cancelQuery()
{
    QtConcurrent::run(this, &Query::killQueryPostgres);
}


void Query::killQueryPostgres()
{
    if (m_database.driverName() == "QPSQL" && m_postgresBackendPID > 0)
    {
        QSqlDatabase kill_db = QSqlDatabase::cloneDatabase(m_database, "CLONED_" + QUuid::createUuid().toString());
        kill_db.open();

        QSqlQuery q(kill_db);
        q.prepare("SELECT pg_cancel_backend(:pid);");
        q.bindValue(":pid", m_postgresBackendPID);
        q.exec();

        kill_db.close();
    }
}

void Query::reconnectDatabase()
{
    //reconnection for running same query twice. seems to be faster.
    if (m_database.isOpen())
        m_database.close();
    m_database.open();

    // set up ability to cancel
    if (m_database.driverName() == "QPSQL")
    {
        QSqlQuery q(m_database);
        q.prepare("SELECT pg_backend_pid();");
        q.exec();
        q.next();
        int pid = q.value(0).toInt();

        if (pid > 0) m_postgresBackendPID = pid;
    } else m_postgresBackendPID = -1;
}

bool Query::executeQuery(const QString query)
{
    if (query.trimmed().isEmpty())
        return false;

    reconnectDatabase();

    m_columnNames.clear();

    m_query = QSqlQuery (m_database);
    m_query.setForwardOnly(true);

    m_startTime = QDateTime::currentDateTime();
    bool success = m_query.exec(query);
    m_endTime = QDateTime::currentDateTime();

    if (success)
    {
        QSqlRecord header = m_query.record();
        for (int col = 0; col < header.count(); ++col)
        {
            m_columnNames.append(header.fieldName(col).toUpper());
        }
    } else
    {
        m_columnNames.clear();
    }

    emit finished();

    return success;
}

QList<QString> Query::getColumNames()
{
    return m_columnNames;
}

int Query::numRowsAffected()
{
    return m_query.numRowsAffected();
}

const QString Query::lastError()
{
    return m_query.lastError().text();
}

const QString Query::lastQuery()
{
    return m_query.lastQuery();
}

const QDateTime Query::endTime()
{
    return m_endTime;
}

const QDateTime Query::startTime()
{
    return m_startTime;
}

bool Query::isSelect()
{
    return m_query.isSelect();
}
