#ifndef QUERYSTOPPER_H
#define QUERYSTOPPER_H

#include "src/Connection.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QSqlError>

class QueryStopper : public QObject
{
    Q_OBJECT

signals:
    void finished();

public:
    explicit QueryStopper(QObject *parent = nullptr);
    ~QueryStopper();

public slots:
    void executeStopSession(Connection connection, int pid);

private:
    QString m_queryConnecionId;

};

#endif // QUERYSTOPPER_H
