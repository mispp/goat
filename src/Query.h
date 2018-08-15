#ifndef QUERY_H
#define QUERY_H

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QStandardItem>
#include <QFuture>
#include <QFutureWatcher>
#include <QDateTime>

typedef QList<QStandardItem*> TableRow;

class Query : public QObject
{
    Q_OBJECT

public:
    Query();
    ~Query();

    void switchDatabase(QString connectionId);
    void cancelQuery();
    bool submitQueryForExecution(const QString query);
    bool displayGrid(); //this is a flag!
    QList<QString> getColumNames();
    int numRowsAffected();
    const QString lastError();
    const QString lastQuery();
    const QDateTime startTime();
    const QDateTime endTime();
    bool isSelect();
    bool isSuccesful();

    const QList<TableRow> getNextRowSet(int rowSetSize);

private:
    QSqlQuery m_query;

    QString m_refConnectionId;
    QSqlDatabase m_database;

    QDateTime m_startTime;
    QDateTime m_endTime;

    QFuture<bool> m_queryFuture;
    QFutureWatcher<void> m_queryFutureWatcher;

    int m_postgresBackendPID;

    QList<QString> m_columnNames;

    void reconnectDatabase();
    void killQueryPostgres();
    bool executeQuery(const QString query);

public slots:
    void queryfinished();

signals:
    void finished();

};

#endif // QUERY_H
