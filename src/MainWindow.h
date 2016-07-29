#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ConnectionManager.h"
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_actionNew_connection_triggered();
	void on_actionExit_triggered();
	void on_tabBarConnections_tabCloseRequested(int index);

	void on_actionNew_query_triggered();

private:
	Ui::MainWindow *ui;
	void closeEvent(QCloseEvent *event);
	void writeSettings();
	void readSettings();

	ConnectionManager* m_connectionManager;
	int m_tabSeq;
	void addTab();
};

#endif // MAINWINDOW_H
