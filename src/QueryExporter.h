#ifndef QUERYEXPORTER_H
#define QUERYEXPORTER_H

#include "src/AbstractQuery.h"
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

class QueryExporter : public AbstractQuery
{
    Q_OBJECT

signals:
    void finished(QStringList message);

public:
    explicit QueryExporter(QObject *parent = nullptr);
    ~QueryExporter();

public slots:
    void executeSql(QString sql, Connection connection, QString outputFilePath, Csv csvHandler);
    void setStopExportFlag(bool newValue);

private:
    bool m_stopExportFlag;
    QMutex m_mutex;

};

#endif // QUERYEXPORTER_H
