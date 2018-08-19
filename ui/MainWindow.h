#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMenu>
#include <QToolButton>

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
    void on_actionNewFile_triggered();
    void on_actionCloseFile_triggered();
    void on_actionOpenFile_triggered();
    void on_currentTabTextChanged();
    void on_actionSaveFileAs_triggered();
    void on_actionSaveFile_triggered();
    void on_actionConnection_Manager_triggered();
    void on_actionQueryBlockAtCursor_triggered();

public slots:
    void on_connectionListToolButtonExpandTriggered();
    void on_connectionListToolButtonItemTriggered(QAction* triggeredAction);
    void on_connectionStateChanged();
    void on_disconnectListToolButtonItemTriggered(QAction* triggeredAction);

    void on_toolbarOrientationChange(Qt::Orientation orientation);

private:
    void closeEvent(QCloseEvent *event);
    void writeSettings();
    void readSettings();
    void invalidateEnabledStates();

	Ui::MainWindow *ui;
    ConnectionManager m_connectionManager;
    QMenu m_connectionListMenu;
    QMenu m_openConnectionListMenu;

    QToolButton* m_connectionManagerToolButton;
    QToolButton* m_disconnectToolButton;
};

#endif // MAINWINDOW_H
