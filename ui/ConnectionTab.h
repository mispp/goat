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

//public slots:
//    void on_ctrlEnter_triggered();

public:
	explicit ConnectionTab(QWidget *parent = 0);
	~ConnectionTab();
    void executeQueryAtCursor(QSqlDatabase sqlDatabase);
    void executeQuery(QSqlDatabase sqlDatabase, QString query);

private:
    Ui::ConnectionTab *ui;
    QSqlQueryModel* m_queryResultsModel;
};

#endif // CONNECTIONTAB_H
