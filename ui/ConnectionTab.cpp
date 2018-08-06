#include "ConnectionTab.h"
#include "ui_ConnectionTab.h"
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

ConnectionTab::ConnectionTab(QString filename, ConnectionManager *connectionManager, QWidget *parent) : m_connectionManager(connectionManager), QWidget(parent), ui(new Ui::ConnectionTab)
{
	ui->setupUi(this);

    m_queryResultsModel = new QSqlQueryModel(this);
    m_openConnectionsModel = new QStandardItemModel(this);
    m_filename = filename;

    ui->resultsGrid->setModel(m_queryResultsModel);
    ui->comboBoxConnections->setModel(m_openConnectionsModel);

    ui->resultsText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    readFile();
    setModified(false);
    refreshOpenConnections();

    connect(ui->codeEditor, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    connect(m_connectionManager, SIGNAL(connectionStateChanged()), this, SLOT(refreshOpenConnections()));
}

ConnectionTab::~ConnectionTab()
{
	delete ui;
}

void ConnectionTab::executeQueryAtCursor(QSqlDatabase sqlDatabase)
{
    executeQuery(sqlDatabase, ui->codeEditor->getQueryAtCursor());
}

void ConnectionTab::executeQuery(QSqlDatabase sqlDatabase, QString query)
{
    if (query.trimmed().isEmpty())
        return;

    ui->resultsText->clear();
    QSqlQuery q(sqlDatabase);

    QDateTime start = QDateTime::currentDateTime();
    bool success = q.exec(query);
    QDateTime end = QDateTime::currentDateTime();
    bool displayGrid = success && q.isSelect();

    if (displayGrid)
    {
        m_queryResultsModel->setQuery(q);
        ui->resultsGrid->resizeColumnsToContents();
        ui->resultsTabBar->setCurrentIndex(0);
    }
    else
    {
        m_queryResultsModel->clear();
        ui->resultsTabBar->setCurrentIndex(1);
    }

    ui->resultsText->appendPlainText("Timestamp: " + end.toString("yyyy-mm-dd hh:mm:ss"));
    ui->resultsText->appendPlainText("Elapsed: " + QString::number(start.msecsTo(end)) + " ms");
    if (success && !displayGrid)
        ui->resultsText->appendPlainText("Number of rows affected: " + QString::number(q.numRowsAffected()));
    else if (!success)
        ui->resultsText->appendPlainText(q.lastError().text());
    ui->resultsText->appendPlainText("");
    ui->resultsText->appendPlainText("Query:");
    ui->resultsText->appendPlainText("-------------------------------");
    ui->resultsText->appendPlainText(q.lastQuery());
}

bool ConnectionTab::modified() const
{
    return ui->codeEditor->document()->isModified();
}

void ConnectionTab::setModified(const bool &modified)
{
    ui->codeEditor->document()->setModified(modified);
}

QString ConnectionTab::filename() const
{
    return m_filename;
}

void ConnectionTab::readFile()
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

void ConnectionTab::writeFile()
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

void ConnectionTab::setFilename(const QString &filename)
{
    m_filename = filename;
}

void ConnectionTab::refreshOpenConnections()
{
    QMap<QString, QString> openConnections = m_connectionManager->getOpenConnections();

    QString usedConnectionId = ui->comboBoxConnections->currentData(Qt::UserRole+1).toString();

    m_openConnectionsModel->clear();

    if (openConnections.count() == 0)
    {
        QStandardItem* openConnectionItem = new QStandardItem();

        openConnectionItem->setText("<no open connections>");

        m_openConnectionsModel->appendRow(openConnectionItem);

        ui->comboBoxConnections->setCurrentIndex(0);
    }
    else
    {
        foreach (QString key, openConnections.keys())
        {
            QStandardItem* openConnectionItem = new QStandardItem();

            openConnectionItem->setText(openConnections[key]);
            openConnectionItem->setData(key, Qt::UserRole+1);

            m_openConnectionsModel->appendRow(openConnectionItem);
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

void ConnectionTab::on_button_selectionQuery_released()
{
    int index = ui->comboBoxConnections->currentIndex();
    QString connectionId = ui->comboBoxConnections->itemData(index, Qt::UserRole+1).toString();

    if (!m_connectionManager->isOpen(connectionId))
        return;

    executeQuery(m_connectionManager->getOpenConnection(connectionId), ui->codeEditor->getSelection());
}
