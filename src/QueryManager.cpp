#include "QueryManager.h"

#include <QSqlRecord>
#include <QSqlError>

QueryManager::QueryManager() : QObject()
{
    m_killPid = -1;
}

QueryManager::~QueryManager()
{

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

        QString sql;

        if (clonedDatabase.driverName() == "QPSQL")
            sql = "select pg_backend_pid();";
        else if (clonedDatabase.driverName() == "QMYSQL")
            sql = "SELECT CONNECTION_ID();";

        /* get postgres cancel query pid */
        if (clonedDatabase.isOpen() && !sql.isEmpty())
        {
            QSqlQuery q(clonedDatabase);
            q.prepare(sql);
            bool result = q.exec();
            if (result)
            {
                q.next();
                int pid = q.value(0).toInt();

                if (pid > 0) m_killPid = pid;
                else m_killPid = -1;
            }
        }
        else
        {
            m_killPid = -1;
        }

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
    QSqlDatabase kill_db = database;

    QString sql;

    if (kill_db.driverName() == "QPSQL")
        sql = "SELECT pg_cancel_backend(:pid);";
    else if (kill_db.driverName() == "QMYSQL")
        sql = "KILL QUERY :pid";
    else
        return;

    if (kill_db.isValid() && m_killPid > 0)
    {
        kill_db.open();

        QSqlQuery q(kill_db);
        q.prepare(sql);
        q.bindValue(":pid", m_killPid);
        q.exec();

        kill_db.close();
    }
}

QList<QString> QueryManager::getColumNames()
{
    return m_columnNames;
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

const QString QueryManager::lastError()
{
    return m_query.lastError().text();
}

const QString QueryManager::lastQuery()
{
    return m_query.lastQuery();
}

const QDateTime QueryManager::startTime()
{
    return m_startTime;
}

const QDateTime QueryManager::endTime()
{
    return m_endTime;
}

bool QueryManager::isSelect()
{
    return m_query.isSelect();
}

bool QueryManager::isSuccess()
{
    return m_querySuccess;
}

int QueryManager::numRowsAffected()
{
    return m_query.numRowsAffected();
}
