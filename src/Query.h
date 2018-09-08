#ifndef QUERY_H
#define QUERY_H

#include "src/AbstractQuery.h"
#include "src/RowSet.h"

#include <QDebug>
#include <QUuid>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>

class Query : public AbstractQuery
{
    Q_OBJECT

signals:
    void finished(bool isSelect, QSqlRecord header, QStringList message);
    void nextRowSet(RowSet);

public:
    explicit Query(ConnectionManager* connectionManager, QObject *parent = nullptr);
    ~Query();

public slots:
    void executeSql(QString sql, Connection connection);
    void requestNextRowSet(int rowCount);

private:


};

#endif // QUERY_H

