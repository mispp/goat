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

    readFile();
    setModified(false);
    refreshOpenConnections();

    connect(ui->codeEditor, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    connect(m_connectionManager, SIGNAL(connectionStateChanged()), this, SLOT(refreshOpenConnections()));
    connect(&m_query, SIGNAL(finished()), this, SLOT(queryFinished()));

    connect(ui->resultsGrid->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(resultsGridSliderAtEnd(int)));
}

QueryTab::~QueryTab()
{
	delete ui;
}

void QueryTab::executeQueryAtCursor()
{
    QString query = ui->codeEditor->getQueryAtCursor();

    if (!query.isEmpty())
    {
        ui->button_selectionQuery->setEnabled(false);
        ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
        ui->resultsText->clear();
        m_queryResultsModel.clear();

        m_query.submitQueryForExecution(query);
    }
}

void QueryTab::executeSelectedQuery()
{
    QString query = ui->codeEditor->getSelection();

    if (!query.isEmpty())
    {
        ui->button_selectionQuery->setEnabled(false);
        ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
        ui->resultsText->clear();
        m_queryResultsModel.clear();

        m_query.submitQueryForExecution(query);
    }
}

void QueryTab::queryFinished()
{
    displayQueryResults();

    ui->button_selectionQuery->setEnabled(true);
    ui->button_stopQuery->setEnabled(!ui->button_selectionQuery->isEnabled());
}

void QueryTab::displayQueryResults()
{
    if (m_query.displayGrid())
    {
        m_queryResultsModel.clear();
        m_queryResultsModel.setColumnCount(m_query.getColumNames().count());

        int columnIndex = 0;
        foreach (QString columnName, m_query.getColumNames())
        {
            m_queryResultsModel.setHeaderData(columnIndex, Qt::Horizontal, columnName);
            ++columnIndex;
        }

        foreach (TableRow row, m_query.getNextRowSet(100))
        {
            m_queryResultsModel.appendRow(row);
        }

        ui->resultsGrid->resizeColumnsToContents();
        ui->resultsTabBar->setCurrentIndex(0);
    }
    else
    {
        m_queryResultsModel.clear();
        ui->resultsTabBar->setCurrentIndex(1);
    }

    ui->resultsText->appendPlainText("Timestamp: " + m_query.startTime().toString("yyyy-MM-dd hh:mm:ss"));
    ui->resultsText->appendPlainText("Elapsed: " + QString::number(m_query.startTime().msecsTo(m_query.endTime())) + " ms");
    if (m_query.isSuccesful() && !m_query.isSelect())
        ui->resultsText->appendPlainText("Number of rows affected: " + QString::number(m_query.numRowsAffected()));
    else if (!m_query.isSuccesful())
        ui->resultsText->appendPlainText(m_query.lastError());
    ui->resultsText->appendPlainText("");
    ui->resultsText->appendPlainText("Query:");
    ui->resultsText->appendPlainText("-------------------------------");
    ui->resultsText->appendPlainText(m_query.lastQuery());
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
    m_query.submitQueryForExecution(ui->codeEditor->getSelection());

    ui->codeEditor->setFocus();
}

void QueryTab::on_button_stopQuery_released()
{
    m_query.cancelQuery();

    ui->codeEditor->setFocus();
}

void QueryTab::on_comboBoxConnections_currentIndexChanged(int index)
{
    QString connectionId = ui->comboBoxConnections->itemData(index, Qt::UserRole+1).toString();
    if (!connectionId.isEmpty())
    {
        m_query.switchDatabase(connectionId);
    }
}

void QueryTab::resultsGridSliderAtEnd(int value)
{
    if (ui->resultsGrid->verticalScrollBar()->maximum() == value)
    {
        foreach (TableRow row, m_query.getNextRowSet(100))
        {
            m_queryResultsModel.appendRow(row);
        }
    }
}
