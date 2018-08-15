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
    QList<QString> getColumNames();
    int numRowsAffected();
    const QString lastError();
    const QString lastQuery();
    const QDateTime startTime();
    const QDateTime endTime();
    bool isSelect();
    const QList<TableRow> getNextRowSet(int rowSetSize);
    bool executeQuery(const QString query);

private:
    QSqlQuery m_query;

    QString m_refConnectionId;
    QSqlDatabase m_database;

    QDateTime m_startTime;
    QDateTime m_endTime;

    int m_postgresBackendPID;

    QList<QString> m_columnNames;

    void reconnectDatabase();
    void killQueryPostgres();

signals:
    void finished();

};

#endif // QUERY_H
