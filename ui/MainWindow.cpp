#include "ui/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/ConnectionDialog.h"
#include "ui/ConnectionTab.h"
#include "ui/AboutDialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardItem>

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool unsavedChanges = false;
    for (int i = 0; i < ui->tabBarConnections->count(); ++i)
    {
        ConnectionTab *connectionTab = ((ConnectionTab*) ui->tabBarConnections->widget(i));
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

void MainWindow::on_tabBarConnections_tabCloseRequested(int index)
{
    ConnectionTab *tab = (ConnectionTab*) ui->tabBarConnections->widget(index);

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

        invalidateEnabledStates();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::on_newFileButton_clicked()
{
    ConnectionTab *connectionTab = new ConnectionTab("", ui->tabBarConnections);
    ui->tabBarConnections->insertTab(ui->tabBarConnections->count(), connectionTab, tr("Untitled"));
    ui->tabBarConnections->setCurrentIndex(ui->tabBarConnections->count()-1);

    connect(connectionTab, SIGNAL(textChanged()), this, SLOT(on_currentTabTextChanged()));
    invalidateEnabledStates();
}

void MainWindow::on_newConnectionButton_clicked()
{
    Connection connection = Connection::defaultConnection();
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
    invalidateEnabledStates();
}

void MainWindow::invalidateEnabledStates()
{
    ConnectionTab *currentTab = ((ConnectionTab*) ui->tabBarConnections->currentWidget());

    ui->actionCloseFile->setDisabled(currentTab == nullptr);
    ui->saveFileButton->setDisabled(currentTab == nullptr);
    ui->actionSaveFile->setDisabled(currentTab == nullptr);
    ui->actionSaveFileAs->setDisabled(currentTab == nullptr);

    int index = ui->connectionComboBox->currentIndex();
    bool connectionAtIndex = index != -1;

    ui->editConnectionButton->setDisabled(!connectionAtIndex);
    ui->actionEditConnection->setDisabled(!connectionAtIndex);
    ui->deleteConnectionButton->setDisabled(!connectionAtIndex);
    ui->actionDeleteConnection->setDisabled(!connectionAtIndex);

    bool isOpen = false;
    bool queryExists = ui->tabBarConnections->currentIndex() != -1;

    if (connectionAtIndex)
    {
        QString connectionId = ui->connectionComboBox->itemData(index).toString();
        Connection connection = m_connectionManager.getConnections()[connectionId];
        isOpen = m_connectionManager.isOpen(connectionId);
    }

    ui->openConnectionButton->setDisabled(isOpen);
    ui->actionOpenConnection->setDisabled(isOpen);
    ui->closeConnectionButton->setDisabled(!isOpen);
    ui->actionCloseConnection->setDisabled(!isOpen);
    ui->queryBlockButton->setDisabled(!connectionAtIndex || !queryExists);
    ui->actionQueryBlockAtCursor->setDisabled(!connectionAtIndex || !queryExists);
//    ui->queryFileButton->setDisabled(!connectionAtIndex || !queryExists);
//    ui->actionQuery_File->setDisabled(!connectionAtIndex || !queryExists);
}

void MainWindow::on_editConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    Connection connection = m_connectionManager.getConnections()[connectionId];
    ConnectionDialog dialog(connection);
    dialog.setWindowTitle(tr("Edit Connection"));
    if(dialog.exec() == QDialog::Accepted)
    {
        connection = dialog.getConnection();
        m_connectionManager.saveConnection(connection);
        ui->connectionComboBox->setItemText(index, connection.name());
        invalidateEnabledStates();
    }
}

void MainWindow::on_deleteConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    m_connectionManager.deleteConnection(connectionId);
    ui->connectionComboBox->removeItem(index);
    invalidateEnabledStates();
}

void MainWindow::on_openConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    m_connectionManager.openConnection(m_connectionManager.getConnections()[connectionId]);
    invalidateEnabledStates();
}

void MainWindow::on_closeConnectionButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();
    m_connectionManager.closeConnection(connectionId);
    invalidateEnabledStates();
}

void MainWindow::on_queryBlockButton_clicked()
{
    int index = ui->connectionComboBox->currentIndex();
    QString connectionId = ui->connectionComboBox->itemData(index).toString();

    if (!m_connectionManager.isOpen(connectionId))
        on_openConnectionButton_clicked();

    if (!m_connectionManager.isOpen(connectionId))
        return;

    ConnectionTab *tab = ((ConnectionTab*) ui->tabBarConnections->currentWidget());
    QSqlDatabase db = m_connectionManager.getOpenConnection(connectionId);
    tab->executeQueryAtCursor(db);
}

void MainWindow::on_actionCloseFile_triggered()
{
    on_tabBarConnections_tabCloseRequested(ui->tabBarConnections->currentIndex());
}

void MainWindow::on_openFileButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("All files (*.*) ;; Sql files (*.sql)"));

    if (!filename.isEmpty())
    {
        QFileInfo fileInfo(filename);
        ConnectionTab *connectionTab = new ConnectionTab(filename, ui->tabBarConnections);
        ui->tabBarConnections->insertTab(ui->tabBarConnections->count(), connectionTab, fileInfo.fileName());
        ui->tabBarConnections->setCurrentIndex(ui->tabBarConnections->count()-1);

        connect(connectionTab, SIGNAL(textChanged()), this, SLOT(on_currentTabTextChanged()));
        invalidateEnabledStates();
    }
}

void MainWindow::changeTabFilename(ConnectionTab *connectionTab)
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As"), QDir::homePath(), tr("All files (*.*) ;; Sql files (*.sql)"));
    connectionTab->setFilename(filename);
}

void MainWindow::saveTab(ConnectionTab *connectionTab)
{
    connectionTab->writeFile();
    QString text = connectionTab->filename();
    if (text.isEmpty())
        text = tr("Untitled");
    else
        text = QFileInfo(connectionTab->filename()).fileName();
    ui->tabBarConnections->setTabText(ui->tabBarConnections->currentIndex(), text);
}

void MainWindow::on_saveFileButton_clicked()
{
    ConnectionTab *connectionTab = ((ConnectionTab*) ui->tabBarConnections->currentWidget());
    if (connectionTab->filename().isEmpty())
    {
        changeTabFilename(connectionTab);
    }
    saveTab(connectionTab);
}

void MainWindow::on_actionSaveFileAs_triggered()
{
    ConnectionTab *connectionTab = ((ConnectionTab*) ui->tabBarConnections->currentWidget());
    changeTabFilename(connectionTab);
    saveTab(connectionTab);
}

void MainWindow::on_currentTabTextChanged()
{
    invalidateEnabledStates(); //save button

    ConnectionTab *connectionTab = ((ConnectionTab*) ui->tabBarConnections->currentWidget());
    if (connectionTab->modified())
    {
        //have tab text show file has been changed
        int index = ui->tabBarConnections->currentIndex();
        QString currentTabText = ui->tabBarConnections->tabText(index);
        if (!currentTabText.startsWith("*"))
            ui->tabBarConnections->setTabText(index, "*" + currentTabText);
    }
}
