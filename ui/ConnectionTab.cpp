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

    m_queryResultsModel = new QSqlQueryModel(this);
    ui->resultsGrid->setModel(m_queryResultsModel);

    ui->resultsText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
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

    if (q.exec(query))
    {
        if (q.size() != -1)
        {
            //size is not -1, so probably an select statement

            m_queryResultsModel->setQuery(q);
            ui->resultsGrid->resizeColumnsToContents();
            ui->resultsTabBar->setCurrentIndex(0);
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
        ui->resultsText->appendPlainText(query);
    }
}

