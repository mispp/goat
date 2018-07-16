#include "ui/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/ConnectionDialog.h"
#include "ui/ConnectionTab.h"
#include "ui/AboutDialog.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSqlError>
#include <QStandardItem>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),	ui(new Ui::MainWindow) {
	ui->setupUi(this);
    this->setFocus(); //HACK otherwise the new file button is focused, which looks weird
	readSettings();
    on_newFileButton_clicked();

    foreach(Connection connection, m_connectionManager.getConnections().values())
    {
        ui->connectionComboBox->addItem(connection.name(), connection.connectionId()); //TODO order this by name?
    }
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_actionExit_triggered() {
	//this->close();
	QApplication::exit();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	/*
		here:
		loop over all code editors and find unsaved changes, then trigger qmessage box
		otherwise just exit
	*/

//	QMessageBox exitConfirmationDialog;
//	exitConfirmationDialog.setWindowTitle("Exit?");
//	exitConfirmationDialog.setText("Are you sure you want to exit?");
//	exitConfirmationDialog.setInformativeText("All unsaved changes will be lost.");
//	exitConfirmationDialog.setIcon(QMessageBox::Warning);
//	exitConfirmationDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//	exitConfirmationDialog.setMinimumSize(QSize(600, 120));
//	QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
//	QGridLayout* layout = (QGridLayout*)exitConfirmationDialog.layout();
//	layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

//	if(exitConfirmationDialog.exec() == QMessageBox::Yes)
	{
		writeSettings();
		event->accept();
    }
//    else
//    {
//		event->ignore();
//	}

}

void MainWindow::readSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "settings");
	settings.beginGroup("MainWindow");

	resize(settings.value("size",  QSize(640, 480)).toSize());
	move(settings.value("position", QPoint(200, 200)).toPoint());

	settings.endGroup();
}


void MainWindow::writeSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "settings");
	settings.beginGroup("MainWindow");

	settings.setValue("size", this->size());
	settings.setValue("position", this->pos());

	settings.endGroup();
}


void MainWindow::refreshFileActions()
{
    ui->actionCloseFile->setDisabled(ui->tabBarConnections->tabBar()->currentIndex() == -1);  //FIXME bug when clicking close button on last tab
}

void MainWindow::on_tabBarConnections_tabCloseRequested(int index)
{
	QWidget *clickedTab = ui->tabBarConnections->widget(index);
	clickedTab->close();
    clickedTab->deleteLater();

    refreshFileActions();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::on_newFileButton_clicked()
{
    ConnectionTab *connectionTab = new ConnectionTab(ui->tabBarConnections);
    ui->tabBarConnections->insertTab(ui->tabBarConnections->count(), connectionTab, "Untitled");
    ui->tabBarConnections->setCurrentIndex(ui->tabBarConnections->count()-1);

    refreshFileActions();
}

void MainWindow::on_newConnectionButton_clicked()
{
    Connection connection;
    ConnectionDialog dialog(connection);
    if(dialog.exec() == QDialog::Accepted)
    {
        connection = dialog.getConnection();
        m_connectionManager.saveConnection(connection);
        ui->connectionComboBox->addItem(connection.name(), connection.connectionId());
        int lastIndex = ui->connectionComboBox->count() - 1;
        ui->connectionComboBox->setCurrentIndex(lastIndex);
    }
}

void MainWindow::on_connectionComboBox_currentIndexChanged(int index)
{
    refreshConnectionActions();
}

void MainWindow::refreshConnectionActions()
{
    int index = ui->connectionComboBox->currentIndex();
    bool connectionAtIndex = index != -1;

    ui->editConnectionButton->setDisabled(!connectionAtIndex);
    ui->actionEditConnection->setDisabled(!connectionAtIndex);
    ui->deleteConnectionButton->setDisabled(!connectionAtIndex);
    ui->actionDeleteConnection->setDisabled(!connectionAtIndex);

    bool isOpen = false;

    if (connectionAtIndex)
    {
        QString connectionId = ui->connectionComboBox->itemData(index).toString();
        Connection connection = m_connectionManager.getConnections()[connectionId];
        isOpen = m_connectionManager.getOpenConnections().contains(connectionId);
    }

    ui->openConnectionButton->setDisabled(isOpen);
    ui->actionOpenConnection->setDisabled(isOpen);
    ui->closeConnectionButton->setDisabled(!isOpen);
    ui->actionCloseConnection->setDisabled(!isOpen);
    ui->queryBlockButton->setDisabled(!isOpen);
    ui->actionQueryBlockAtCursor->setDisabled(!isOpen);
//    ui->queryFileButton->setDisabled(!isOpen);
//    ui->actionQuery_File->setDisabled(!isOpen);
}

void MainWindow::on_editConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    Connection connection = m_connectionManager.getConnections()[connectionId];
    ConnectionDialog dialog(connection);
    if(dialog.exec() == QDialog::Accepted)
    {
        connection = dialog.getConnection();
        m_connectionManager.saveConnection(connection);
        ui->connectionComboBox->setItemText(index, connection.name());
        refreshConnectionActions();
    }
}

void MainWindow::on_deleteConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    m_connectionManager.deleteConnection(connectionId);
    ui->connectionComboBox->removeItem(index);
    refreshConnectionActions();
}

void MainWindow::on_openConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    m_connectionManager.openConnection(m_connectionManager.getConnections()[connectionId]);
    refreshConnectionActions();
}

void MainWindow::on_closeConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    m_connectionManager.closeConnection(connectionId);
    refreshConnectionActions();
}

void MainWindow::on_queryBlockButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    QSqlDatabase db = m_connectionManager.getOpenConnections()[connectionId];
    ConnectionTab *tab = ((ConnectionTab*) ui->tabBarConnections->currentWidget());
    tab->executeQueryAtCursor(db);
}

void MainWindow::on_actionCloseFile_triggered()
{
    ui->tabBarConnections->removeTab(ui->tabBarConnections->currentIndex());
}
