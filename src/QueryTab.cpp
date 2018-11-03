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

QueryTab::QueryTab(QString filename, ConnectionManager *connectionManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionTab),
    m_connectionManager(connectionManager),
    m_tabId("TAB_" + QUuid::createUuid().toString()),
    m_filename(filename)
{
	ui->setupUi(this);

    qRegisterMetaType<QSqlRecord>("QSqlRecord");

    ui->resultsGrid->setModel(&m_queryResultsModel);
    ui->comboBoxConnections->setModel(&m_openConnectionsModel);
    ui->resultsText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->button_stopQuery->setEnabled(false);
    ui->button_stopExport->setEnabled(false);

    readFile();
    setModified(false);
    refreshOpenConnections();

    //handle file changes
    connect(ui->codeEditor, &CodeEditor::textChanged, this, &QueryTab::textChanged);

    //handle connection changes
    connect(m_connectionManager, &ConnectionManager::connectionStateChanged, this, &QueryTab::refreshOpenConnections);

    //get thread for query to be displayed
    m_queryThread = new QThread();
    m_query = new Query(m_connectionManager);
    m_query->moveToThread(m_queryThread);
    connect(m_queryThread, &QThread::finished, m_queryThread, &QThread::deleteLater);
    connect(m_queryThread, &QThread::finished, m_query, &Query::deleteLater);
    connect(this, &QueryTab::executeSql, m_query, &Query::executeSql); //push sql to query object
    connect(m_query, &Query::finished, this, &QueryTab::on_queryFinished); //when successfull
    connect(m_query, &Query::failed, this, &QueryTab::on_queryFailed); //when fail
    connect(this, &QueryTab::requestNextRowSet, m_query, &Query::requestNextRowSet); //push request for more rows
    connect(m_query, &Query::nextRowSet, this, &QueryTab::on_rowSetReceived); //handle results
    m_queryThread->start();

    //get thread for exporting
    m_queryExporterThread = new QThread();
    m_queryExporter = new QueryExporter(m_connectionManager);
    m_queryExporter->moveToThread(m_queryExporterThread);
    connect(m_queryExporterThread, &QThread::finished, m_queryExporterThread, &QThread::deleteLater);
    connect(m_queryExporterThread, &QThread::finished, m_queryExporter, &Query::deleteLater);
    connect(this, &QueryTab::requestQueryExport, m_queryExporter, &QueryExporter::executeSql);
    connect(m_queryExporter, &QueryExporter::finished, this, &QueryTab::on_exportFinished);
    connect(m_queryExporter, &QueryExporter::failed, this, &QueryTab::on_exportFinished);
    m_queryExporterThread->start();

    //handle tableview
    connect(ui->resultsGrid->verticalScrollBar(), &QScrollBar::valueChanged, this, &QueryTab::on_resultsGridSliderAtEnd);
}

QueryTab::~QueryTab()
{
	delete ui;

    m_queryThread->quit();
    m_queryExporterThread->quit();
}

QString QueryTab::filename() const
{
    return m_filename;
}

void QueryTab::setFilename(const QString &filename)
{
    m_filename = filename;
}

bool QueryTab::modified() const
{
    return ui->codeEditor->document()->isModified();
}

void QueryTab::setModified(const bool &modified)
{
    ui->codeEditor->document()->setModified(modified);
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

/************************************************************************************************************************************************************/

void QueryTab::runQueryAtCursor()
{
    QString connectionId = ui->comboBoxConnections->currentData(Qt::UserRole+1).toString();
    if (!connectionId.isEmpty())
    {
        Connection connection = m_connectionManager->getConnections()[connectionId];

        //if (m_queryFuture.isFinished())
        {
            submitQueryForExecution(ui->codeEditor->getQueryAtCursor(), connection);
        }
    }
}

void QueryTab::runSelectedQuery()
{
    QString connectionId = ui->comboBoxConnections->currentData(Qt::UserRole+1).toString();
    if (!connectionId.isEmpty())
    {
        Connection connection = m_connectionManager->getConnections()[connectionId];

        //if (m_queryFuture.isFinished())
        {
            submitQueryForExecution(ui->codeEditor->getSelection(), connection);
        }
    }
}

bool QueryTab::isFinished()
{
    return m_query->isFinished() && m_queryExporter->isFinished();
}

void QueryTab::stopActivities()
{
    if (!m_query->isFinished())
    {
        QueryStopper stopper;
        stopper.executeStopSession(m_query->connection(), m_query->sessionPid());
    }

    if (!m_queryExporter->isFinished())
    {
        on_button_stopExport_released();
    }
}

void QueryTab::submitQueryForExecution(const QString query, const Connection connection)
{
    if (!query.isEmpty())
    {
        ui->button_selectionQuery->setEnabled(false);
        ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
        ui->resultsText->clear();
        m_queryResultsModel.clear();


        emit executeSql(query, connection);
    }
}

void QueryTab::on_queryFinished(bool isSelect, QSqlRecord header, QStringList message)
{
    if (isSelect)
    {
        QFont font = ui->resultsGrid->horizontalHeader()->font();
        QFontMetrics fm(font);

        for(int i=0; i<header.count(); ++i)
        {
            QString fieldName = header.field(i).name().toUpper();

            m_queryResultsModel.setHorizontalHeaderItem(i, new QStandardItem(fieldName));
            ui->resultsGrid->setColumnWidth(i, fm.width(fieldName)+40); // fit to column width
        }

        foreach (QString line, message)
        {
            ui->resultsText->appendPlainText(line);
        }

        emit requestNextRowSet(100);

        ui->resultsTabBar->setCurrentIndex(0);
    }
    else
    {
        foreach (QString line, message)
        {
            ui->resultsText->appendPlainText(line);
        }

        ui->resultsTabBar->setCurrentIndex(1);
    }

    ui->button_selectionQuery->setEnabled(true);
    ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
}

void QueryTab::on_queryFailed(QStringList message)
{
    foreach (QString line, message)
    {
        ui->resultsText->appendPlainText(line);
    }

    ui->button_selectionQuery->setEnabled(true);
    ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
    ui->resultsTabBar->setCurrentIndex(1);
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
        ui->button_selectionQuery->setEnabled(false);
    }
    else
    {
        foreach (QString key, openConnections.keys())
        {
            QStandardItem* openConnectionItem = new QStandardItem();

            openConnectionItem->setText(openConnections[key]);
            openConnectionItem->setData(key, Qt::UserRole+1);
            //setIcon(QIcon(":/icons/papirus/apps/" + m_driver));
            openConnectionItem->setIcon(QIcon(":/icons/papirus/apps/" + QSqlDatabase::database(key).driverName()));

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

        ui->button_selectionQuery->setEnabled(true);
    }
}


/*******************************************************************************************************************************************************/

void QueryTab::on_resultsGridSliderAtEnd(int value)
{
    if (ui->resultsGrid->verticalScrollBar()->maximum() == value)
    {
        emit requestNextRowSet(100);
    }
}

void QueryTab::on_rowSetReceived(RowSet rowSet)
{
    foreach(Row row, rowSet)
    {
        m_queryResultsModel.appendRow(row);
    }
}

void QueryTab::on_button_selectionQuery_released()
{
    runSelectedQuery();

    ui->codeEditor->setFocus();
}

void QueryTab::on_button_stopQuery_released()
{
    qDebug()<< "stop button pressed";

    if (!m_query->isFinished())
    {
        QueryStopper stopper;
        stopper.executeStopSession(m_query->connection(), m_query->sessionPid());

        ui->codeEditor->setFocus();
    }
}

void QueryTab::on_button_exportQueryResults_released()
{
    ExportQueryDialog dialog(&m_queryResultsModel, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString filename = dialog.outputFilePath();

        if (filename.isEmpty() || !m_query->isFinished() || !m_query->isSelect())
            return;

        QString delimiter = dialog.delimiter() == "<tab>" ? "\t" : dialog.delimiter();
        bool includeHeader = dialog.includeHeader();
        DataFormatter dataFormatter = DataFormatter(dialog.formatOverrides());

        Csv csv(delimiter, dialog.quoteSymbol(), includeHeader, dialog.alwaysQuoteStrings(), dialog.replaceNewLine(), dataFormatter);

        ui->button_exportQueryResults->setEnabled(false);
        ui->button_stopExport->setEnabled(true);

        emit requestQueryExport(m_query->lastQuery(), m_query->connection(), filename, csv);
    }
}

void QueryTab::on_button_stopExport_released()
{
    if (!m_queryExporter->isFinished())
    {
        m_queryExporter->setStopExportFlag(true); //true means "yes, interupt the export loop"

        QueryStopper stopper;

        stopper.executeStopSession(m_queryExporter->connection(), m_queryExporter->sessionPid());
    }
}

void QueryTab::on_exportFinished(QStringList)
{
    ui->button_exportQueryResults->setEnabled(true);
    ui->button_stopExport->setEnabled(false);
}
