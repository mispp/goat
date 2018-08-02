#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "src/ConnectionManager.h"

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
	void on_actionExit_triggered();
	void on_tabBarConnections_tabCloseRequested(int index);
	void on_actionNew_query_triggered();
    void on_action_Open_triggered();
    void on_action_showConnectionManager_triggered();

    void on_actionAbout_triggered();

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
