#ifndef QUERYEXPORTER_H
#define QUERYEXPORTER_H

#include "src/Connection.h"
#include "src/Csv.h"

#include <QDebug>
#include <QObject>
#include <QUuid>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include <QSqlRecord>
#include <QMutex>
#include <QFile>

class QueryExporter : public QObject
{
    Q_OBJECT

signals:
    void queryExecutionFinished(QStringList message);
    void queryExecutionFailed(QStringList message);

public:
    explicit QueryExporter(QObject *parent = nullptr);
    ~QueryExporter();

    bool isFinished();

public slots:
    void executeSqlAndExport(QString sql, Connection connection, QString outputFilePath, Csv csvHandler);
    void stopExport();

private:
    QString m_queryConnecionId;
    QSqlQuery m_query;
    //QList<QSqlField> m_header;
    bool m_stopFlag;

};

#endif // QUERYEXPORTER_H
