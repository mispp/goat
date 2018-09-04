#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include "../src/ConnectionManager.h"
#include "../src/Query.h"
#include "../src/QueryExporter.h"
#include "../src/Csv.h"

#include <QPlainTextEdit>
#include <QString>
#include <QWidget>
#include <QWidget>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSplitter>
#include <QComboBox>
#include <QSqlQuery>
#include <QFuture>
#include <QFutureWatcher>
#include <QDateTime>
#include <QThread>
#include <QUuid>

namespace Ui {
class ConnectionTab;
}

class QueryTab : public QWidget
{
	Q_OBJECT

signals:
   void textChanged();
   //void connectionSwitched(const QString connectionId);
   void executeSql(QString sql, Connection connection);
   void requestNextRowSet(int rowCount);
   void requestQueryExport(QString sql, Connection connection, QString outputFilePath, Csv csvHandler);
   void requestExportStop();

public:
    explicit QueryTab(QString filename, ConnectionManager *connectionManager, QWidget *parent = 0);
    ~QueryTab();

    QString filename() const;
    void setFilename(const QString &filename);
    bool modified() const;
    void setModified(const bool &modified);
    void readFile();
    void writeFile();

    void runQueryAtCursor();
    void runSelectedQuery();
    bool isFinished();

private:
    Ui::ConnectionTab *ui;

    ConnectionManager* m_connectionManager;
    QString m_tabId;
    QString m_filename;

    QStandardItemModel m_queryResultsModel;
    QStandardItemModel m_openConnectionsModel;

    Query* m_query;
    QThread* m_queryThread;

    QueryExporter* m_queryExporter;
    QThread* m_queryExporterThread;

    void submitQueryForExecution(const QString query, const Connection connection);

public slots:
    void refreshOpenConnections();
    void on_queryFinished(bool isSelect, QSqlRecord header, QStringList message);
    void on_queryFailed(QStringList message);
    void on_resultsGridSliderAtEnd(int value);
    void on_rowSetReceived(RowSet rowSet);

private slots:
    void on_button_selectionQuery_released();
    void on_button_stopQuery_released();
    void on_button_exportQueryResults_released();
    void on_button_stopExport_released();
};

#endif // CONNECTIONTAB_H
