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

public slots:
    void executeSql(QString sql, Connection connection);
    void executeSqlAndExport(QString sql, Connection connection, QString outputFilePath, Csv csvHandler);
    void requestNextRowSet(int rowCount);
    void stopExport();

private:
    QString m_queryConnecionId;
    QSqlQuery m_query;
    QList<QSqlField> m_header;
    bool m_stopFlag;

};

#endif // QUERY_H

