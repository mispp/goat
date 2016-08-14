#include "ConnectionTab.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QAction>
#include "CodeEditor.h"
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
#include "ConnectionManager.h"

ConnectionTab::ConnectionTab(QWidget *parent) : QSplitter(parent)
{
	setOrientation(Qt::Vertical);

	m_model = new QSqlQueryModel(this);

	m_codeEditor = new CodeEditor(this);

	m_frame = new QFrame(this);
	m_frame->setLayout(new QVBoxLayout());

	m_comboBox = new QComboBox();
	m_comboBox->setModel(ConnectionManager::getInstance()->getModel());

	QFrame* h_frame = new QFrame(m_frame);
	QHBoxLayout* h_layout= new QHBoxLayout();
	h_frame->setLayout(h_layout);
	h_layout->addWidget(m_comboBox);
	h_layout->addStretch();

	m_frame->layout()->addWidget(h_frame);
	m_frame->layout()->addWidget(m_codeEditor);
	addWidget(m_frame);

	//create tab container
	m_tabBar = new QTabWidget(this);

	//create tab results and the control which will show results
	m_resultsTab = new QWidget(m_tabBar);
	m_resultsTab->setLayout(new QVBoxLayout(m_resultsTab));
	m_resultsGrid = new QTableView(m_resultsTab);
	m_resultsTab->layout()->addWidget(m_resultsGrid);
	m_resultsGrid->setModel(m_model);

	//create tab for messages and the control which will show messages
	m_messagesTab = new QWidget(m_tabBar);
	m_messagesTab->setLayout(new QVBoxLayout(m_messagesTab));
	m_messagesText = new QPlainTextEdit(m_messagesTab);
	m_messagesTab->layout()->addWidget(m_messagesText);
	m_messagesText->setStyleSheet("QPlainTextEdit { font-family: Courier }");

	//add both widgets to tab
	m_tabBar->addTab(m_resultsGrid, "Results");
	m_tabBar->addTab(m_messagesTab, "Messages");

	addWidget(m_tabBar);

	QList<int> sectionSizes = sizes();
	sectionSizes[0] = parent->height()*0.6;
	sectionSizes[1] = parent->height() - sectionSizes[0];
	setSizes(sectionSizes);

	/*
	m_codeEditor->setFocus();
	QTextCursor tmpCursor = m_codeEditor->textCursor();
	tmpCursor.movePosition(QTextCursor::Start);
	m_codeEditor->setTextCursor(tmpCursor);
	*/

	setConnection();
}

QString ConnectionTab::getDriverName()
{
	return m_database.driverName();
}

void ConnectionTab::setConnection()
{
	QVariant v_item = m_comboBox->currentData();
	m_database = QSqlDatabase::database(ConnectionManager::getInstance()->getDefaultConnection()->getConnectionId());
}

void ConnectionTab::on_ctrlEnter_triggered()
{
	const QString query = m_codeEditor->getSqlStatement().trimmed();
	m_model->clear();
	m_messagesText->clear();

	QSqlQuery q(m_database);
	//q.setForwardOnly(true);
	q.prepare(query);

	if (q.exec() == true)
	{
		if (q.size() != -1)
		{
			//size is not -1, so probably an select statement
			qDebug() << "Number of rows in result set: " + QString::number(q.size());

			m_model->setQuery(q);
			m_resultsGrid->resizeColumnsToContents();

			m_tabBar->setCurrentIndex(0);
		} else
		{
			//not a select statement

			m_tabBar->setCurrentIndex(1);
			m_messagesText->appendPlainText("Timestamp: " + QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss"));
			m_messagesText->appendPlainText("Number of rows affected: " + QString::number(q.numRowsAffected()));
			m_messagesText->appendPlainText("");
			m_messagesText->appendPlainText(q.executedQuery());
		}
	}
	else
	{
		m_tabBar->setCurrentIndex(1);
		m_messagesText->appendPlainText(q.lastError().text());
		m_messagesText->appendPlainText("");
		m_messagesText->appendPlainText("Query:");
		m_messagesText->appendPlainText("-------------------------------");
		m_messagesText->appendPlainText(q.executedQuery());
	}
}

QString ConnectionTab::getConnectionId()
{
	return m_database.connectionName();
}
