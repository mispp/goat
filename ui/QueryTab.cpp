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

QueryTab::QueryTab(QString filename, ConnectionManager *connectionManager, QWidget *parent) : m_connectionManager(connectionManager), QWidget(parent), ui(new Ui::ConnectionTab)
{
	ui->setupUi(this);

    m_filename = filename;

    ui->resultsGrid->setModel(&m_queryResultsModel);
    ui->comboBoxConnections->setModel(&m_openConnectionsModel);

    ui->resultsText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    readFile();
    setModified(false);
    refreshOpenConnections();

    connect(ui->codeEditor, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    connect(m_connectionManager, SIGNAL(connectionStateChanged()), this, SLOT(refreshOpenConnections()));
}

QueryTab::~QueryTab()
{
	delete ui;
}

void QueryTab::executeQueryAtCursor(QSqlDatabase sqlDatabase)
{
    ui->resultsText->clear();
    m_queryResultsModel.clear();
    m_futureQueryExecutionStatus = QtConcurrent::run(this, &QueryTab::executeQuery, sqlDatabase, ui->codeEditor->getQueryAtCursor());
}

void QueryTab::executeSelectedQuery(QSqlDatabase sqlDatabase)
{
    ui->resultsText->clear();
    m_queryResultsModel.clear();
    m_futureQueryExecutionStatus = QtConcurrent::run(this, &QueryTab::executeQuery, sqlDatabase, ui->codeEditor->getSelection());
}

void QueryTab::executeQuery(QSqlDatabase sqlDatabase, QString query)
{
    qDebug() << "execution in new thread started";

    if (query.trimmed().isEmpty())
        return;

    m_sqlQuery = QSqlQuery (sqlDatabase);
    //m_sqlQuery.setForwardOnly(true);

    QDateTime start = QDateTime::currentDateTime();
    bool success = m_sqlQuery.exec(query);
    QDateTime end = QDateTime::currentDateTime();

    //if query is not cancelled
    displayQueryResults(success, start, end);
}

void QueryTab::displayQueryResults(bool success, QDateTime start, QDateTime end)
{
    bool displayGrid = success && m_sqlQuery.isSelect();

    if (displayGrid)
    {
        m_queryResultsModel.setQuery(m_sqlQuery);
        ui->resultsGrid->resizeColumnsToContents();
        ui->resultsTabBar->setCurrentIndex(0);
    }
    else
    {
        m_queryResultsModel.clear();
        ui->resultsTabBar->setCurrentIndex(1);
    }

    ui->resultsText->appendPlainText("Timestamp: " + end.toString("yyyy-mm-dd hh:mm:ss"));
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
}
