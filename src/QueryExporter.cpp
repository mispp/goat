#include "QueryExporter.h"

QueryExporter::QueryExporter(QObject *parent) :
    QObject(parent),
    m_queryConnecionId("CLONED_EXPORT_" + QUuid::createUuid().toString()),
    m_stopFlag(false)
{
    qDebug() << "queryexporter created with id: " + m_queryConnecionId;
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
    return false;
}

void QueryExporter::executeSqlAndExport(QString sql, Connection connection, QString outputFilePath, Csv csvHandler)
{
    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }

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
}

void QueryExporter::stopExport()
{
    QMutex mutex;
    mutex.lock();
    m_stopFlag = true;
    mutex.unlock();
}
