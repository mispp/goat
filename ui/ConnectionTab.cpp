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
    ui->codeEditor->setStyleSheet("QPlainTextEdit { font: 18px; }");
    ui->comboBoxConnections->setModel(ConnectionManager::getInstance()->getEstablishedConnectionModel());

    m_queryResultsModel = new QSqlQueryModel(this);
    ui->resultsGrid->setModel(m_queryResultsModel);

    m_establishedConnection = QSqlDatabase::database(ui->comboBoxConnections->itemData(0, Qt::UserRole+1).toString());

    highlighter = new Highlighter(ui->codeEditor->document());

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), ui->codeEditor), SIGNAL(activated()), this, SLOT(on_ctrlEnter_triggered()));
}

ConnectionTab::~ConnectionTab()
{
	delete ui;
}

void ConnectionTab::on_comboBoxConnections_currentIndexChanged(int index)
{
    m_establishedConnection = QSqlDatabase::database(ConnectionManager::getInstance()->getEstablishedConnectionModel()->item(index)->data(Qt::UserRole+1).toString());
}

void ConnectionTab::on_ctrlEnter_triggered()
{
    runQuery(getQueryAtCursor(ui->codeEditor->textCursor().block()));
}

void ConnectionTab::on_button_selectionQuery_released()
{
    runQuery(ui->codeEditor->textCursor().selectedText().trimmed());
}

void ConnectionTab::runQuery(const QString query)
{
    if (!query.trimmed().isEmpty())
    {
        executeQuery(query.trimmed().simplified());
    }
}

QString ConnectionTab::getQueryAtCursor(const QTextBlock & block)
{
    //https://stackoverflow.com/questions/50995743/qplaintextedit-qtextcursor-how-to-get-lines-above-and-below-cursor-until-abo
    QTextBlock before = block;
    QTextBlock after = block;

    if(block.text().trimmed().isEmpty()) return "";

    QStringList lines{block.text()};

    do
    {
        before = before.previous();
        if(before.text().trimmed().isEmpty()) break;
        lines.prepend(before.text());
    }
    while(before.isValid());

    do
    {
        after = after.next();
        if(after.text().trimmed().isEmpty()) break;
        lines.append(after.text());
    }
    while(after.isValid());

    return lines.join("\n");
}

void ConnectionTab::executeQuery(const QString query)
{
    m_queryResultsModel->clear();
    ui->resultsText->clear();

    QSqlQuery q(m_establishedConnection);
    //q.setForwardOnly(true);
    q.prepare(query);

    if (q.exec() == true)
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
        ui->resultsText->appendPlainText(q.executedQuery());
    }
}
