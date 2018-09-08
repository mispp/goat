#include "QueryExporter.h"

QueryExporter::QueryExporter(ConnectionManager* connectionManager, QObject *parent) :
    AbstractQuery(connectionManager, parent),
    m_stopExportFlag(false)
{

}

QueryExporter::~QueryExporter()
{

}

void QueryExporter::executeSql(QString sql, Connection connection, QString outputFilePath, Csv csvHandler)
{
    setStopExportFlag(false);

    m_isFinished = false;

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
