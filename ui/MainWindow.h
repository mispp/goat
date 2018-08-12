#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "src/Connection.h"
#include "src/ConnectionManager.h"
#include "ui/QueryTab.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

    void changeTabFilename(QueryTab *connectionTab);
    void saveTab(QueryTab *connectionTab);

private slots:
	void on_actionExit_triggered();
	void on_tabBarConnections_tabCloseRequested(int index);
    void on_actionAbout_triggered();
    void on_newFileButton_clicked();
    void on_newConnectionButton_clicked();
    void on_connectionComboBox_currentIndexChanged(int index);
    void on_editConnectionButton_clicked();
    void on_deleteConnectionButton_clicked();
    void on_openConnectionButton_clicked();
    void on_closeConnectionButton_clicked();
    void on_actionCloseFile_triggered();
    void on_openFileButton_clicked();
    void on_saveFileButton_clicked();
    void on_currentTabTextChanged();
    void on_actionSaveFileAs_triggered();

    void on_actionConnection_Manager_triggered();
    void on_actionQueryBlockAtCursor_triggered();

    void on_button_connectionManager_released();

private:
    void closeEvent(QCloseEvent *event);
    void writeSettings();
    void readSettings();
    void invalidateEnabledStates();

	Ui::MainWindow *ui;
    ConnectionManager m_connectionManager;
};

#endif // MAINWINDOW_H
