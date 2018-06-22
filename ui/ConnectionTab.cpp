#include "ConnectionTab.h"
#include "ui_ConnectionTab.h"
#include "src/ConnectionManager.h"

#include <QTabWidget>
#include <QSqlQueryModel>
#include <QShortcut>
#include <QSplitter>
#include <QTextCursor>
#include <QTextBlock>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QTabBar>
#include <QDateTime>
#include <QComboBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QAction>

ConnectionTab::ConnectionTab(QWidget *parent) :	QWidget(parent), ui(new Ui::ConnectionTab)
{
	ui->setupUi(this);
    ui->comboBoxConnections->setModel(ConnectionManager::getInstance()->getEstablishedConnectionModel());

    m_queryResultsModel = new QSqlQueryModel(this);
    ui->resultsGrid->setModel(m_queryResultsModel);

    //setConnection();
    m_establishedConnection = QSqlDatabase::database(ui->comboBoxConnections->itemData(0, Qt::UserRole+1).toString());

	// insert test-sql code
    //ui->codeEditor->setStyleSheet("QPlainTextEdit { font-family: Courier }");

	connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), ui->codeEditor), SIGNAL(activated()), this, SLOT(onctrlEnter_triggered()));
}

ConnectionTab::~ConnectionTab()
{
	delete ui;
}

void ConnectionTab::on_comboBoxConnections_currentIndexChanged(int index)
{
    //ConnectionManager::getInstance()->getModel()->item(index)->data(Qt::UserRole).toString();

    //QMap<QString, QVariant> connectionDefinition = ConnectionManager::getInstance()->getEstablishedConnectionModel()->item(index)->data(Qt::UserRole).value<QMap<QString, QVariant>>();
    m_establishedConnection = QSqlDatabase::database(ui->comboBoxConnections->currentData(Qt::UserRole+1).toString());
}

QString ConnectionTab::getConnectionId()
{
    return m_establishedConnection.connectionName();
}

QString ConnectionTab::getDriverName()
{
    return m_establishedConnection.driverName();
}

void ConnectionTab::setConnection()
{
    //QVariant v_item = ui->comboBoxConnections->currentData();
    m_establishedConnection = QSqlDatabase::database();
}

void ConnectionTab::onctrlEnter_triggered()
{
	const QString query = getSqlStatement(ui->codeEditor).trimmed();
    m_queryResultsModel->clear();
	ui->resultsText->clear();

    qDebug() << "Established connection count: " + QString::number(QSqlDatabase::connectionNames().count());

    QSqlQuery q(m_establishedConnection);
	//q.setForwardOnly(true);
	q.prepare(query);

	if (q.exec() == true)
	{
		if (q.size() != -1)
		{
			//size is not -1, so probably an select statement
			qDebug() << "Number of rows in result set: " + QString::number(q.size());

            m_queryResultsModel->setQuery(q);
			ui->resultsGrid->resizeColumnsToContents();

			ui->resultsTabBar->setCurrentIndex(0);
			//resultsGrid->setVisible(false);
			//resultsGrid->setVisible(true);
		} else
		{
			//not a select statement

			ui->resultsTabBar->setCurrentIndex(1);
			ui->resultsText->appendPlainText("Timestamp: " + QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss"));
			ui->resultsText->appendPlainText("Number of rows affected: " + QString::number(q.numRowsAffected()));
			ui->resultsText->appendPlainText("");
			ui->resultsText->appendPlainText(q.executedQuery());
		}
	}
	else
	{
		ui->resultsTabBar->setCurrentIndex(1);
		ui->resultsText->appendPlainText(q.lastError().text());
		ui->resultsText->appendPlainText("");
		ui->resultsText->appendPlainText("Query:");
		ui->resultsText->appendPlainText("-------------------------------");
		ui->resultsText->appendPlainText(q.executedQuery());
	}
}

QString ConnectionTab::getSqlStatement(QPlainTextEdit* codeEditor)
{
	QTextCursor original_cursor_position = codeEditor->textCursor();
	QTextCursor cursorBefore = original_cursor_position;
	QTextCursor cursorAfter = original_cursor_position;

	QString before = "";
	QString at = "";
	QString after = "";

	at = original_cursor_position.block().text().trimmed();

	while (cursorBefore.block().previous().text().trimmed() != "")
	{
		cursorBefore.movePosition(QTextCursor::Up);
		before = cursorBefore.block().text() + " " + before;
	}

	while (cursorAfter.block().next().text().trimmed() != "")
	{
		cursorAfter.movePosition(QTextCursor::Down);
		after = after + " " + cursorAfter.block().text();
	}

	return before + " " + at + " " + after;
}



