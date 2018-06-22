#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include <QPlainTextEdit>
#include <QString>
#include <QWidget>
#include <QWidget>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSplitter>
#include <QComboBox>

namespace Ui {
class ConnectionTab;
}

class ConnectionTab : public QWidget
{
	Q_OBJECT

public slots:
	void onctrlEnter_triggered();

public:
	explicit ConnectionTab(QWidget *parent = 0);
	~ConnectionTab();
	QString getDriverName();
	QString getConnectionId();

private slots:
	void on_comboBoxConnections_currentIndexChanged(int index);

private:
	Ui::ConnectionTab *ui;

    QSqlDatabase m_establishedConnection;
    QSqlQueryModel* m_queryResultsModel;

	void setConnection();

	QString getSqlStatement(QPlainTextEdit* codeEditor);
};

#endif // CONNECTIONTAB_H
