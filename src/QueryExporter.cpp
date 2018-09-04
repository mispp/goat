#include "QueryExporter.h"

QueryExporter::QueryExporter(QObject *parent) :
    QObject(parent),
    m_queryConnecionId("CLONED_EXPORT_" + QUuid::createUuid().toString()),
    m_stopFlag(false),
    m_isFinished(true),
    m_sessionPid(-1)
{

}

QueryExporter::~QueryExporter()
{
    m_query.finish();

    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }
}

bool QueryExporter::isFinished()
{
    return m_isFinished;
}

Connection QueryExporter::connection()
{
    return m_connection;
}

int QueryExporter::sessionPid()
{
    return m_sessionPid;
}

void QueryExporter::executeSqlAndExport(QString sql, Connection connection, QString outputFilePath, Csv csvHandler)
{
    m_connection = connection;

    QMutex mutex;

    mutex.lock();
    m_stopFlag = false;
    mutex.unlock();

    m_isFinished = false;

    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }

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

        m_query = QSqlQuery(clonedDatabase);
        m_query.setForwardOnly(true);

        bool queryOK = m_query.exec(sql);

        if (queryOK)
        {
            QStringList message;

            QFile file(outputFilePath);

            if (!file.open(QFile::WriteOnly|QFile::Truncate))
            {
                message.append("Cannot open file " + outputFilePath + " for writing.");
                emit queryExecutionFailed(message);
            }
            else
            {
                QTextStream stream(&file);
                csvHandler.write(&stream, &m_query, &m_stopFlag);
                emit queryExecutionFinished(message);
            }
        }
        else
        {
            QStringList message;
            message.append(m_query.lastError().text());

            emit queryExecutionFailed(message);
        }

        m_query.finish();
    }

    m_isFinished = true;
    m_sessionPid = -1;
}

void QueryExporter::stopExport()
{
    QMutex mutex;
    mutex.lock();
    m_stopFlag = true;
    mutex.unlock();
}

int QueryExporter::getSessionPid(QSqlDatabase clonedDatabase)
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
