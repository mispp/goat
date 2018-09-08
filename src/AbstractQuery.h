#ifndef ABSTRACTQUERY_H
#define ABSTRACTQUERY_H

#include "src/Connection.h"

#include <QObject>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlDatabase>
#include <QUuid>

class AbstractQuery : public QObject
{
    Q_OBJECT

signals:
    virtual void finished();
    void failed(QStringList message);

public:
    explicit AbstractQuery(QObject *parent = nullptr);
    ~AbstractQuery();

    bool isFinished();
    bool isSelect();
    Connection connection();
    int sessionPid();
    QString lastQuery();

public slots:
    //virtual void executeSql() = 0;

protected:
    const QString m_queryConnecionId;
    bool m_isFinished;
    QSqlQuery m_query;
    Connection m_connection;
    int m_sessionPid;
    QList<QSqlField> m_header;

    int getSessionPid(QSqlDatabase clonedDatabase);
};

#endif // ABSTRACTQUERY_H
