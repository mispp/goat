#ifndef QUERY_H
#define QUERY_H

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QStandardItem>
#include <QFuture>
#include <QFutureWatcher>
#include <QDateTime>

typedef QList<QStandardItem*> TableRow;

class QueryManager : public QObject
{
    Q_OBJECT

public:
    QueryManager();
    ~QueryManager();

    void switchDatabase(QSqlDatabase database);
    void cancelQuery(QSqlDatabase database);
    QList<QString> getColumNames();
    int numRowsAffected();
    const QString lastError();
    const QString lastQuery();
    const QDateTime startTime();
    const QDateTime endTime();
    bool isSelect();
    bool isSuccess();
    const QList<TableRow> getNextRowSet(int rowSetSize);
    bool executeQuery(QSqlDatabase database, const QString query);

private:
    QString m_queryString;
    QSqlQuery m_query;
    bool m_querySuccess;

    QString m_clonedConnectionId;

    QDateTime m_startTime;
    QDateTime m_endTime;

    int m_postgresBackendPID;

    QList<QString> m_columnNames;

    void killQueryPostgres(QSqlDatabase database, int pid);

    QMutex m_mutexExecution;
    QMutex m_mutexRetrieval;

signals:
    void finished();

};

#endif // QUERY_H
