#ifndef QUERY_H
#define QUERY_H

#include "src/Connection.h"
#include "src/Csv.h"
#include "src/RowSet.h"
#include "src/ConnectionManager.h"

#include <QDebug>
#include <QObject>
#include <QUuid>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include <QSqlRecord>

class Query : public QObject
{
    Q_OBJECT

signals:
    void queryExecutionFinished(bool isSelect, QSqlRecord header, QStringList message);
    void queryExecutionFailed(QStringList message);
    void nextRowSet(RowSet);

public:
    explicit Query(QObject *parent = nullptr);
    ~Query();

    bool isFinished();
    bool isSelect();
    Connection connection();
    QString lastQuery();

public slots:
    void executeSql(QString sql, Connection connection);
    void requestNextRowSet(int rowCount);

private:
    QString m_queryConnecionId;
    bool m_isFinished;
    QSqlQuery m_query;
    QList<QSqlField> m_header;
    Connection m_connection;

};

#endif // QUERY_H

