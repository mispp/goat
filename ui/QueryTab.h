#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include "../src/ConnectionManager.h"

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

namespace Ui {
class ConnectionTab;
}

class QueryTab : public QWidget
{
	Q_OBJECT

signals:
   void textChanged();

public:
    explicit QueryTab(QString filename, ConnectionManager *connectionManager, QWidget *parent = 0);
    ~QueryTab();
    void executeQueryAtCursor(QSqlDatabase sqlDatabase);
    void executeSelectedQuery(QSqlDatabase sqlDatabase);
    bool executeQuery(QSqlDatabase sqlDatabase, QString query);
    void displayQueryResults(bool success, QDateTime start, QDateTime end);
    QString filename() const;
    void setFilename(const QString &filename);
    bool modified() const;
    void setModified(const bool &modified);
    void readFile();
    void writeFile();

private:
    Ui::ConnectionTab *ui;
    QSqlQueryModel m_queryResultsModel;
    QStandardItemModel m_openConnectionsModel;
    QString m_filename;
    ConnectionManager* m_connectionManager;

    QSqlQuery m_sqlQuery;
    QDateTime m_sqlQueryStart;
    QDateTime m_sqlQueryEnd;
    QSqlDatabase m_database;

    QFuture<bool> m_queryFuture;
    QFutureWatcher<void> m_queryFutureWatcher;

    int m_postgresBackendPID;

public slots:
    void refreshOpenConnections();
    void queryFinished();
private slots:
    void on_button_selectionQuery_released();

    void on_button_stopQuery_released();
    void on_comboBoxConnections_currentIndexChanged(int index);
};

#endif // CONNECTIONTAB_H
