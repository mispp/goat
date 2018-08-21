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
    bool executeQuery(QSqlDatabase database, const QString query);
    void cancelQuery(QSqlDatabase database);
    QList<QString> getColumNames();
    const QList<TableRow> getNextRowSet(int rowSetSize);
    const QString lastError();
    const QString lastQuery();
    const QDateTime startTime();
    const QDateTime endTime();
    bool isSelect();
    bool isSuccess();
    int numRowsAffected();

private:
    bool m_querySuccess;
    int m_killPid;
    QString m_queryString;
    QSqlQuery m_query;
    QString m_clonedConnectionId;
    QDateTime m_startTime;
    QDateTime m_endTime;
    QList<QString> m_columnNames;

signals:
    void finished();

};

#endif // QUERY_H
