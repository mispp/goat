#include "QueryExporter.h"

QueryExporter::QueryExporter(QObject *parent) :
    AbstractQuery(parent),
    m_stopExportFlag(false)
{

}

QueryExporter::~QueryExporter()
{

}

void QueryExporter::executeSql(QString sql, Connection connection, QString outputFilePath, Csv csvHandler)
{
    m_connection = connection;

    setStopExportFlag(false);

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

        emit failed(errorList);
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
                emit failed(message);
            }
            else
            {
                QTextStream stream(&file);
                csvHandler.write(&stream, &m_query, &m_stopExportFlag);
                emit finished(message);
            }
        }
        else
        {
            QStringList message;
            message.append(m_query.lastError().text());

            emit failed(message);
        }

        m_query.finish();
    }

    m_isFinished = true;
    m_sessionPid = -1;

    setStopExportFlag(false);
}

void QueryExporter::setStopExportFlag(bool newValue)
{
    m_mutex.lock();
    m_stopExportFlag = newValue;
    m_mutex.unlock();
}
