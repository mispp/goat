#include "QueryTab.h"
#include "ui_QueryTab.h"
#include "src/ConnectionManager.h"

#include <QAction>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QSplitter>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlResult>
#include <QTabBar>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>
#include <QSqlRecord>
#include <QScrollBar>

QueryTab::QueryTab(QString filename, ConnectionManager *connectionManager, QWidget *parent) : m_connectionManager(connectionManager), QWidget(parent), ui(new Ui::ConnectionTab)
{
	ui->setupUi(this);

    m_filename = filename;

    ui->resultsGrid->setModel(&m_queryResultsModel);
    ui->comboBoxConnections->setModel(&m_openConnectionsModel);
    ui->resultsText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    ui->button_stopQuery->setEnabled(false);

    //m_queryFutureWatcher.setFuture(m_queryFuture);

    readFile();
    setModified(false);
    refreshOpenConnections();

    connect(ui->codeEditor, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    connect(m_connectionManager, SIGNAL(connectionStateChanged()), this, SLOT(refreshOpenConnections()));
    connect(&m_queryFutureWatcher, SIGNAL(finished()), this, SLOT(queryFinished()));

    connect(ui->resultsGrid->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(resultsGridSliderAtEnd(int)));
}

QueryTab::~QueryTab()
{
	delete ui;
}

void QueryTab::executeQueryAtCursor(QSqlDatabase sqlDatabase)
{
    QString query = ui->codeEditor->getQueryAtCursor();

    if (!query.isEmpty())
    {
        ui->button_selectionQuery->setEnabled(false);
        ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
        ui->resultsText->clear();
        m_queryResultsModel.clear();

        m_queryFuture = QtConcurrent::run(this, &QueryTab::executeQuery, sqlDatabase, query);
        m_queryFutureWatcher.setFuture(m_queryFuture);
    }
}

void QueryTab::executeSelectedQuery(QSqlDatabase sqlDatabase)
{
    QString query = ui->codeEditor->getSelection();

    if (!query.isEmpty())
    {
        ui->button_selectionQuery->setEnabled(false);
        ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
        ui->resultsText->clear();
        m_queryResultsModel.clear();

        m_queryFuture = QtConcurrent::run(this, &QueryTab::executeQuery, sqlDatabase, query);
        m_queryFutureWatcher.setFuture(m_queryFuture);
    }
}

bool QueryTab::executeQuery(QSqlDatabase sqlDatabase, QString query)
{
    if (query.trimmed().isEmpty())
        return false;

    reconnectDatabase();

    m_sqlQuery = QSqlQuery (m_database); //here it stops now (actually it takes a lot of time to finish)
    m_sqlQuery.setForwardOnly(true);

    m_sqlQueryStart = QDateTime::currentDateTime();
    bool success = m_sqlQuery.exec(query); //if previous assignment is moved to on_combobox_index_changed, then it stops here
    m_sqlQueryEnd = QDateTime::currentDateTime();

    return success;
}

void QueryTab::queryFinished()
{
    displayQueryResults(m_queryFuture.result(), m_sqlQueryStart, m_sqlQueryEnd);

    ui->button_selectionQuery->setEnabled(true);
    ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
}

void QueryTab::displayQueryResults(bool success, QDateTime start, QDateTime end)
{
    bool displayGrid = success && m_sqlQuery.isSelect();

    if (displayGrid)
    {
        m_queryResultsModel.clear();

        QSqlRecord record = m_sqlQuery.record(); //this is only header, pointer is still at the invalid row

        int columnCount = record.count();

        m_queryResultsModel.setColumnCount(columnCount);
        for (int col = 0; col < record.count(); ++col)
        {
            m_queryResultsModel.setHeaderData(col, Qt::Horizontal, record.fieldName(col).toUpper());
        }

        loadChunk(100);

        ui->resultsGrid->resizeColumnsToContents();
        ui->resultsTabBar->setCurrentIndex(0);
    }
    else
    {
        m_queryResultsModel.clear();
        ui->resultsTabBar->setCurrentIndex(1);
    }

    ui->resultsText->appendPlainText("Timestamp: " + end.toString("yyyy-MM-dd hh:mm:ss"));
    ui->resultsText->appendPlainText("Elapsed: " + QString::number(start.msecsTo(end)) + " ms");
    if (success && !displayGrid)
        ui->resultsText->appendPlainText("Number of rows affected: " + QString::number(m_sqlQuery.numRowsAffected()));
    else if (!success)
        ui->resultsText->appendPlainText(m_sqlQuery.lastError().text());
    ui->resultsText->appendPlainText("");
    ui->resultsText->appendPlainText("Query:");
    ui->resultsText->appendPlainText("-------------------------------");
    ui->resultsText->appendPlainText(m_sqlQuery.lastQuery());
}

bool QueryTab::modified() const
{
    return ui->codeEditor->document()->isModified();
}

void QueryTab::setModified(const bool &modified)
{
    ui->codeEditor->document()->setModified(modified);
}

QString QueryTab::filename() const
{
    return m_filename;
}

void QueryTab::readFile()
{
    if (m_filename.isEmpty())
        return;

    ui->codeEditor->document()->clear();

    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error reading file: " + m_filename;

        QMessageBox messageBox;
        messageBox.setWindowTitle("Error");
        messageBox.setText("Error reading file: " + m_filename);
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setStandardButtons(QMessageBox::Ok);

        messageBox.exec();
        return;
    }

    ui->codeEditor->document()->setPlainText(QString(file.readAll()));
    file.close();
}

void QueryTab::writeFile()
{
    if (m_filename.isEmpty())
    {
        return;
    }

    QFile file(m_filename);
    bool error = !file.open(QIODevice::WriteOnly | QIODevice::Text);

    if (!error)
    {
        error = file.write(ui->codeEditor->document()->toPlainText().toUtf8()) == -1;
        file.close();
    }

    if (error)
    {
        qDebug() << "Error writing file: " + m_filename;

        QMessageBox messageBox;
        messageBox.setWindowTitle("Error");
        messageBox.setText("Error writing file: " + m_filename);
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setStandardButtons(QMessageBox::Ok);

        messageBox.exec();
    }
    else
    {
        setModified(false);
    }
}

void QueryTab::setFilename(const QString &filename)
{
    m_filename = filename;
}

void QueryTab::refreshOpenConnections()
{
    QMap<QString, QString> openConnections = m_connectionManager->getOpenConnections();

    QString usedConnectionId = ui->comboBoxConnections->currentData(Qt::UserRole+1).toString();

    m_openConnectionsModel.clear();

    if (openConnections.count() == 0)
    {
        QStandardItem* openConnectionItem = new QStandardItem();

        openConnectionItem->setText("<no open connections>");

        m_openConnectionsModel.appendRow(openConnectionItem);

        ui->comboBoxConnections->setCurrentIndex(0);
    }
    else
    {
        foreach (QString key, openConnections.keys())
        {
            QStandardItem* openConnectionItem = new QStandardItem();

            openConnectionItem->setText(openConnections[key]);
            openConnectionItem->setData(key, Qt::UserRole+1);

            m_openConnectionsModel.appendRow(openConnectionItem);
        }

        if (!usedConnectionId.isEmpty())
        {
            int index = ui->comboBoxConnections->findData(usedConnectionId, Qt::UserRole+1);
            if (index)
            {
                ui->comboBoxConnections->setCurrentIndex(index);
            }
        }
    }
}

void QueryTab::on_button_selectionQuery_released()
{
    int index = ui->comboBoxConnections->currentIndex();
    QString connectionId = ui->comboBoxConnections->itemData(index, Qt::UserRole+1).toString();

    if (!m_connectionManager->isOpen(connectionId))
        return;

    executeSelectedQuery(m_connectionManager->getOpenConnection(connectionId));

    ui->codeEditor->setFocus();
}

void QueryTab::on_button_stopQuery_released()
{
    QtConcurrent::run(m_connectionManager, &ConnectionManager::killQueryPostgres, m_database, m_postgresBackendPID);

    /*if (m_sqlQuery.isActive())
    {
        m_sqlQuery.finish();
    }*/

    ui->codeEditor->setFocus();
}

void QueryTab::on_comboBoxConnections_currentIndexChanged(int index)
{
    m_database.close(); //close previously cloned connection

    QString connectionId = ui->comboBoxConnections->itemData(index, Qt::UserRole+1).toString();
    if (!connectionId.isEmpty())
    {
        m_database = QSqlDatabase::cloneDatabase(m_connectionManager->getOpenConnection(connectionId), "CLONED_" + QUuid::createUuid().toString());
        m_database.open();

        m_postgresBackendPID = -1;
    }
}


void QueryTab::reconnectDatabase()
{
    //reconnection for running same query twice. seems to be faster.
    if (m_database.isOpen())
        m_database.close();
    m_database.open();

    // set up ability to cancel
    if (m_database.driverName() == "QPSQL")
    {
        QSqlQuery q(m_database);
        q.prepare("SELECT pg_backend_pid();");
        q.exec();
        q.next();
        int pid = q.value(0).toInt();

        if (pid > 0) m_postgresBackendPID = pid;
    } else m_postgresBackendPID = -1;
}

void QueryTab::resultsGridSliderAtEnd(int value)
{
    if (ui->resultsGrid->verticalScrollBar()->maximum() == value)
    {
        loadChunk(100);
    }
}

void QueryTab::loadChunk(int size)
{
    int counter = 0;

    while (counter < size && m_sqlQuery.next())
    {
        QSqlRecord record = m_sqlQuery.record();

        QList<QStandardItem*> row;

        for (int col = 0; col < record.count(); ++col)
        {
            QStandardItem* item = new QStandardItem();
            item->setData(record.value(col), Qt::DisplayRole);
            row.append(item);
        }

        m_queryResultsModel.appendRow(row);

        ++counter;
    }
}
