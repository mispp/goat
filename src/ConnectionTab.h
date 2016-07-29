#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include "CodeEditor.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSplitter>
#include <QComboBox>

class ConnectionTab : public QSplitter
{
	Q_OBJECT

public slots:
	void on_ctrlEnter_triggered();

public:
	ConnectionTab(QWidget *parent);
	QString getDriverName();
	QString getConnectionId();

private:
	QSqlDatabase m_database;
	QSqlQueryModel* m_model;

	QTabWidget* m_tabBar;

	QFrame* m_frame;

	QComboBox* m_comboBox;

	CodeEditor* m_codeEditor;

	QWidget* m_resultsTab;
	QTableView* m_resultsGrid;

	QWidget* m_messagesTab;
	QPlainTextEdit* m_messagesText;

	void setConnection();
};

#endif // CONNECTIONTAB_H
