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

#include "Highlighter.h"

namespace Ui {
class ConnectionTab;
}

class ConnectionTab : public QWidget
{
	Q_OBJECT

public slots:
    void on_ctrlEnter_triggered();

public:
	explicit ConnectionTab(QWidget *parent = 0);
	~ConnectionTab();

private slots:
	void on_comboBoxConnections_currentIndexChanged(int index);
    void on_button_selectionQuery_released();

private:
	Ui::ConnectionTab *ui;

    QSqlDatabase m_establishedConnection;
    QSqlQueryModel* m_queryResultsModel;
    void runQuery(const QString query);
    void executeQuery(const QString query);
    Highlighter* highlighter;
    QString getQueryAtCursor(const QTextBlock & block);

};

#endif // CONNECTIONTAB_H
