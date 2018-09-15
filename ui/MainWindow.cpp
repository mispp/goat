#include "ui/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/QueryTab.h"
#include "ui/AboutDialog.h"
#include "ui/ConnectionManagerDialog.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    this->setFocus(); //HACK otherwise the new file button is focused, which looks weird
	readSettings();
    on_actionNewFile_triggered();

    m_connectionListMenu.setTitle("Available connections");
    m_openConnectionListMenu.setTitle("Open connections");
    m_openConnectionListMenu.setEnabled(false);

    ui->toolBar->addAction(ui->actionNewFile);
    ui->toolBar->addAction(ui->actionOpenFile);
    ui->toolBar->addAction(ui->actionSaveFile);
    ui->toolBar->addSeparator();


    /* set connection manager tool button */
    m_connectionManagerToolButton = new QToolButton(ui->toolBar);
    m_connectionManagerToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_connectionManagerToolButton->setMenu(&m_connectionListMenu);
    m_connectionManagerToolButton->setIcon(QIcon(":/icons/silk/icons/silk/connect.png"));
    m_connectionManagerToolButton->setToolTip("Connection manager");
    ui->toolBar->addWidget(m_connectionManagerToolButton);

    /* set disconnect tool button */
    m_disconnectToolButton = new QToolButton(ui->toolBar);
    m_disconnectToolButton->setPopupMode(QToolButton::InstantPopup);
    m_disconnectToolButton->setMenu(&m_openConnectionListMenu);
    m_disconnectToolButton->setIcon(QIcon(":/icons/silk/icons/silk/disconnect.png"));
    m_disconnectToolButton->setToolTip("Disconnect");
    m_disconnectToolButton->setEnabled(false);
    ui->toolBar->addWidget(m_disconnectToolButton);

    addAction(ui->actionQueryBlockAtCursor);
    ui->actionQueryBlockAtCursor->setEnabled(true);

    /* add open and available connections menus (i know it's redundant, but still) */
    ui->menuConnection->addSeparator();
    ui->menuConnection->addMenu(&m_connectionListMenu);
    ui->menuConnection->addMenu(&m_openConnectionListMenu);

    connect(&m_connectionListMenu, SIGNAL(aboutToShow()), this, SLOT(on_connectionListToolButtonExpandTriggered()));
    connect(&m_connectionListMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_connectionListToolButtonItemTriggered(QAction*)));
    connect(&m_openConnectionListMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_disconnectListToolButtonItemTriggered(QAction*)));
    connect(&m_connectionManager, SIGNAL(connectionStateChanged()), this, SLOT(on_connectionStateChanged()));
    connect(m_connectionManagerToolButton, SIGNAL(released()), this, SLOT(on_actionConnection_Manager_triggered()));
    connect(ui->toolBar, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(on_toolbarOrientationChange(Qt::Orientation)));
}

MainWindow::~MainWindow()
{
	delete ui;

    foreach (QString connectionId, QSqlDatabase::connectionNames())
    {
        QSqlDatabase::removeDatabase(connectionId);
    }
}

void MainWindow::on_actionExit_triggered()
{
	this->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool unsavedChanges = false;
    for (int i = 0; i < ui->tabBarConnections->count(); ++i)
    {
        QueryTab *connectionTab = ((QueryTab*) ui->tabBarConnections->widget(i));
        unsavedChanges |= connectionTab->modified();
    }

    QMessageBox exitConfirmationDialog;
    exitConfirmationDialog.setWindowTitle(tr("Exit?"));
    exitConfirmationDialog.setText(tr("Are you sure you want to exit?"));
    exitConfirmationDialog.setInformativeText(tr("All unsaved changes will be lost."));
    exitConfirmationDialog.setIcon(QMessageBox::Warning);
    exitConfirmationDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    exitConfirmationDialog.setMinimumSize(QSize(600, 120));
    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)exitConfirmationDialog.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    //TODO option to ignore this warning in the future

    if(!unsavedChanges || exitConfirmationDialog.exec() == QMessageBox::Yes)
	{
		writeSettings();
		event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::readSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "settings");
	settings.beginGroup("MainWindow");

	resize(settings.value("size",  QSize(640, 480)).toSize());
	move(settings.value("position", QPoint(200, 200)).toPoint());

	settings.endGroup();
}


void MainWindow::writeSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "settings");
	settings.beginGroup("MainWindow");

	settings.setValue("size", this->size());
	settings.setValue("position", this->pos());

	settings.endGroup();
}

void MainWindow::on_tabBarConnections_tabCloseRequested(int index)
{
    QueryTab *tab = (QueryTab*) ui->tabBarConnections->widget(index);

    if (tab == nullptr)
            return;

    if (!tab->isFinished())
    {
        QMessageBox killQueryMsgBox;
        killQueryMsgBox.setWindowTitle("Cancel query?");
        killQueryMsgBox.setText(tr("A query is still running."));
        killQueryMsgBox.setInformativeText(tr("Query will be forcefuly stopped. Do you wish to continue?"));
        killQueryMsgBox.setIcon(QMessageBox::Warning);
        killQueryMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        killQueryMsgBox.setMinimumSize(QSize(600, 120));
        //QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        //QGridLayout* layout = (QGridLayout*)closeConfirmationDialog.layout();

        if (killQueryMsgBox.exec()== QMessageBox::Yes)
        {
            tab->stopActivities();
        }
        else return;
    }

    QMessageBox closeConfirmationDialog;
    closeConfirmationDialog.setWindowTitle(tr("Close?"));
    closeConfirmationDialog.setText(tr("Are you sure you want to close?"));
    closeConfirmationDialog.setInformativeText(tr("All unsaved changes will be lost."));
    closeConfirmationDialog.setIcon(QMessageBox::Warning);
    closeConfirmationDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    closeConfirmationDialog.setMinimumSize(QSize(600, 120));
    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)closeConfirmationDialog.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    //TODO option to ignore this warning in the future

    if(!tab->modified() || closeConfirmationDialog.exec() == QMessageBox::Yes)
    {
        ui->tabBarConnections->removeTab(index);

        tab->close();
        tab->deleteLater();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::on_actionNewFile_triggered()
{
    QueryTab *connectionTab = new QueryTab("", &m_connectionManager, ui->tabBarConnections);
    ui->tabBarConnections->insertTab(ui->tabBarConnections->count(), connectionTab, tr("Untitled"));
    ui->tabBarConnections->setCurrentIndex(ui->tabBarConnections->count()-1);

    connect(connectionTab, SIGNAL(textChanged()), this, SLOT(on_currentTabTextChanged()));
}

void MainWindow::on_actionCloseFile_triggered()
{
    on_tabBarConnections_tabCloseRequested(ui->tabBarConnections->currentIndex());
}

void MainWindow::on_actionOpenFile_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("Sql files (*.sql) ;; All files (*.*)"));

    if (!filename.isEmpty())
    {
        int index = ui->tabBarConnections->currentIndex();
        if (index >= 0)
        {
            //can we "replace" the current tab with the file being opened?
            QueryTab *queryTab = (QueryTab*) ui->tabBarConnections->widget(index);
            if (!queryTab->modified() && queryTab->filename().isEmpty())
                on_tabBarConnections_tabCloseRequested(index);
        }

        QFileInfo fileInfo(filename);
        QueryTab *connectionTab = new QueryTab(filename, &m_connectionManager, ui->tabBarConnections);
        ui->tabBarConnections->insertTab(ui->tabBarConnections->count(), connectionTab, fileInfo.fileName());
        ui->tabBarConnections->setCurrentIndex(ui->tabBarConnections->count()-1);

        connect(connectionTab, SIGNAL(textChanged()), this, SLOT(on_currentTabTextChanged()));
    }
}

void MainWindow::changeTabFilename(QueryTab *connectionTab)
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As"), QDir::homePath(), tr("Sql files (*.sql) ;; All files (*.*)"));
    connectionTab->setFilename(filename);
}

void MainWindow::saveTab(QueryTab *connectionTab)
{
    connectionTab->writeFile();
    QString text = connectionTab->filename();
    if (text.isEmpty())
        text = tr("Untitled");
    else
        text = QFileInfo(connectionTab->filename()).fileName();
    ui->tabBarConnections->setTabText(ui->tabBarConnections->currentIndex(), text);
}

void MainWindow::on_actionSaveFile_triggered()
{
    QueryTab *connectionTab = ((QueryTab*) ui->tabBarConnections->currentWidget());

    if (connectionTab == nullptr)
            return;

    if (connectionTab->filename().isEmpty())
    {
        changeTabFilename(connectionTab);
    }
    saveTab(connectionTab);
}

void MainWindow::on_actionSaveFileAs_triggered()
{
    QueryTab *connectionTab = ((QueryTab*) ui->tabBarConnections->currentWidget());
    changeTabFilename(connectionTab);
    saveTab(connectionTab);
}

void MainWindow::on_currentTabTextChanged()
{
    QueryTab *connectionTab = ((QueryTab*) ui->tabBarConnections->currentWidget());

    if (connectionTab == nullptr)
            return;

    if (connectionTab->modified())
    {
        //have tab text show file has been changed
        int index = ui->tabBarConnections->currentIndex();
        QString currentTabText = ui->tabBarConnections->tabText(index);
        if (!currentTabText.startsWith("*"))
            ui->tabBarConnections->setTabText(index, "*" + currentTabText);
    }
}

void MainWindow::on_actionConnection_Manager_triggered()
{
    ConnectionManagerDialog dialog(&m_connectionManager);
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::on_actionQueryBlockAtCursor_triggered()
{
    QueryTab *tab = ((QueryTab*) ui->tabBarConnections->currentWidget());
    tab->runQueryAtCursor();
}

void MainWindow::on_connectionListToolButtonExpandTriggered()
{
    m_connectionListMenu.clear();

    foreach (Connection connection, m_connectionManager.getConnections().values())
    {
        QAction* establishConnectionAction = new QAction();
        establishConnectionAction->setText(connection.name());
        establishConnectionAction->setData(connection.connectionId());
        establishConnectionAction->setIcon(QIcon(":/icons/db/" + connection.driver()));

        m_connectionListMenu.addAction(establishConnectionAction);
    }
}

void MainWindow::on_connectionListToolButtonItemTriggered(QAction* triggeredAction)
{
    m_connectionManager.openConnection(m_connectionManager.getConnections()[triggeredAction->data().toString()]);
}

void MainWindow::on_connectionStateChanged()
{
    m_openConnectionListMenu.clear();

    foreach (Connection connection, m_connectionManager.getConnections().values())
    {
        if (QSqlDatabase::database(connection.connectionId()).isOpen())
        {
            QAction* disconnectConnectionAction = new QAction();
            disconnectConnectionAction->setText(connection.name());
            disconnectConnectionAction->setData(connection.connectionId());
            disconnectConnectionAction->setIcon(QIcon(":/icons/db/" + connection.driver()));

            m_openConnectionListMenu.addAction(disconnectConnectionAction);
        }
    }

    if (m_openConnectionListMenu.actions().count() > 0)
    {
        m_disconnectToolButton->setEnabled(true);
        m_openConnectionListMenu.setEnabled(m_disconnectToolButton->isEnabled());
    }
    else
    {
        m_disconnectToolButton->setEnabled(false);
        m_openConnectionListMenu.setEnabled(m_disconnectToolButton->isEnabled());
    }
}

void MainWindow::on_disconnectListToolButtonItemTriggered(QAction* triggeredAction)
{
    m_connectionManager.closeConnection(triggeredAction->data().toString());
}

void MainWindow::on_toolbarOrientationChange(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical)
    {
        qDebug() << "is vertical now";
        //we could change button-with-menu width here, so in vertical toolbar it looks better.
        //type of popup needs to be also changed, maybe immediate?
    }
    else if (orientation == Qt::Horizontal)
    {
        qDebug() << "is horizontal now";
        //change it back here
    }
}


void MainWindow::on_tabBarConnections_currentChanged(int index)
{
    QueryTab *currentTab = ((QueryTab*) ui->tabBarConnections->widget(index));

    ui->actionCloseFile->setDisabled(currentTab == nullptr);
    ui->actionSaveFile->setDisabled(currentTab == nullptr);
    ui->actionSaveFileAs->setDisabled(currentTab == nullptr);
    ui->actionQueryBlockAtCursor->setDisabled(currentTab == nullptr);
}
