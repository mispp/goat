#include "QueryStopper.h"

QueryStopper::QueryStopper(QObject *parent) :
    QObject(parent),
    m_queryConnecionId("CLONED_EXPORT_" + QUuid::createUuid().toString())
{

}

QueryStopper::~QueryStopper()
{
    if (QSqlDatabase::contains(m_queryConnecionId))
    {
        QSqlDatabase::database(m_queryConnecionId).close();
        QSqlDatabase::removeDatabase(m_queryConnecionId);
    }
}

void QueryStopper::executeStopSession(Connection connection, int pid)
{
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

        //report failure to cancel?
    }
    else
    {
        QString sql;

        if (clonedDatabase.driverName() == "QPSQL")
            sql = "SELECT pg_cancel_backend(:pid);";
        else if (clonedDatabase.driverName() == "QMYSQL")
            sql = "KILL QUERY :pid";
        else if (clonedDatabase.driverName() == "QODBC")
            sql = "KILL :pid";
        else
            return;

        if (clonedDatabase.isValid() && clonedDatabase.isOpen() && pid > 0)
        {
            QSqlQuery q(clonedDatabase);
            q.prepare(sql);
            q.bindValue(":pid", pid);
            q.exec();

            clonedDatabase.close();
        }
    }

    emit finished();
}
