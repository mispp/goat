#include "QueryManager.h"

#include <QtConcurrent/QtConcurrent>
#include <QSqlRecord>
#include <QSqlError>

QueryManager::QueryManager() : QObject()
{
    m_postgresBackendPID = -1;
}

QueryManager::~QueryManager()
{
    QSqlDatabase::database(m_clonedConnectionId).close();

    QSqlDatabase::removeDatabase(m_clonedConnectionId);
}

void QueryManager::switchDatabase(QSqlDatabase database)
{
    QSqlDatabase clonedDatabase = database;

    m_query = QSqlQuery (clonedDatabase);
    m_query.setForwardOnly(true);


}

bool QueryManager::executeQuery(QSqlDatabase database, const QString query)
{
    if (query.trimmed().isEmpty())
        return false;

    QSqlDatabase clonedDatabase = database;

    m_columnNames.clear();

    if (clonedDatabase.isOpen())
    {
        clonedDatabase.close();
        clonedDatabase.open();

        /* get postgres cancel query pid */
        if (clonedDatabase.isOpen() && clonedDatabase.driverName() == "QPSQL")
        {
            QSqlQuery q(clonedDatabase);
            q.prepare("select pg_backend_pid();");
            bool result = q.exec();
            if (result)
            {
                q.next();
                int pid = q.value(0).toInt();

                if (pid > 0) m_postgresBackendPID = pid;
                else m_postgresBackendPID = -1;
            }
        }

        qDebug() << "cancel pid is: " + QString::number(m_postgresBackendPID);

        m_startTime = QDateTime::currentDateTime();
        m_querySuccess = m_query.exec(query);
        m_endTime = QDateTime::currentDateTime();

        if (m_querySuccess)
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

        return m_querySuccess;
    }
    else
    {
        return false;
    }
}

void QueryManager::cancelQuery(QSqlDatabase database)
{
    killQueryPostgres(database, m_postgresBackendPID);
}


void QueryManager::killQueryPostgres(QSqlDatabase database, int pid)
{
    QSqlDatabase kill_db = database;

    if (kill_db.isValid() && kill_db.driverName() == "QPSQL" && pid > 0)
    {
        kill_db.open();

        QSqlQuery q(kill_db);
        q.prepare("SELECT pg_cancel_backend(:pid);");
        q.bindValue(":pid", m_postgresBackendPID);
        q.exec();

        kill_db.close();
    }
}

QList<QString> QueryManager::getColumNames()
{
    return m_columnNames;
}

int QueryManager::numRowsAffected()
{
    return m_query.numRowsAffected();
}

const QString QueryManager::lastError()
{
    return m_query.lastError().text();
}

const QString QueryManager::lastQuery()
{
    return m_query.lastQuery();
}

const QDateTime QueryManager::endTime()
{
    return m_endTime;
}

const QDateTime QueryManager::startTime()
{
    return m_startTime;
}

bool QueryManager::isSelect()
{
    return m_query.isSelect();
}

bool QueryManager::isSuccess()
{
    return m_querySuccess;
}

const QList<TableRow> QueryManager::getNextRowSet(int rowSetSize)
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
