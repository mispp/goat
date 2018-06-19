#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "NewConnectionDialog.h"
#include "Connection.h"
#include "ui/ConnectionTab.h"

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

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "settings");
	readSettings();
	m_tabSeq = 1;
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_actionNew_connection_triggered() {
	NewConnectionDialog dialog;
	dialog.setModal(true);
	if (dialog.exec())
	{
		//qDebug() << "OK Clicked, creating tab";

        //Connection* connection = dialog.getConnection();
        //ConnectionManager::getInstance()->establishConnection(connection);

        if (QSqlDatabase::connectionNames().count() > 0)
		{
            //qDebug() << "Numer of established connections: " + QString::number(QSqlDatabase::connectionNames().count());
            addTab();
        }

	} else
	{
		//qDebug() << "Cancelled";
	}
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

	QMessageBox exitConfirmationDialog;
	exitConfirmationDialog.setWindowTitle("Exit?");
	exitConfirmationDialog.setText("Are you sure you want to exit?");
	exitConfirmationDialog.setInformativeText("All unsaved changes will be lost.");
	exitConfirmationDialog.setIcon(QMessageBox::Warning);
	exitConfirmationDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	exitConfirmationDialog.setMinimumSize(QSize(600, 120));
	QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout* layout = (QGridLayout*)exitConfirmationDialog.layout();
	layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

	if(exitConfirmationDialog.exec() == QMessageBox::Yes)
	{
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}

}



void MainWindow::readSettings()
{
	qDebug() << "reading settings";

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "settings");
	settings.beginGroup("MainWindow");

	resize(settings.value("size",  QSize(640, 480)).toSize());
	move(settings.value("position", QPoint(200, 200)).toPoint());

	settings.endGroup();
}


void MainWindow::writeSettings()
{
	qDebug() << "updating settings";

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "settings");
	settings.beginGroup("MainWindow");

	settings.setValue("size", this->size());
	settings.setValue("position", this->pos());
	settings.setValue("connectionBarSize", ui->mainSplitter->sizes()[0]);

	settings.endGroup();
}


void MainWindow::on_tabBarConnections_tabCloseRequested(int index)
{
	QWidget *clickedTab = ui->tabBarConnections->widget(index);
	clickedTab->close();
	clickedTab->deleteLater();
}

void MainWindow::on_actionNew_query_triggered() {
    addTab();
    /*if (ConnectionManager::getInstance()->connectionsAvailable())
    {
        addTab();
    } else {
        // error dialog?
    }*/
}

void MainWindow::addTab() {
	QString tabName = "Tab " + QString::number(m_tabSeq);
	++m_tabSeq;

	ConnectionTab *connectionTab = new ConnectionTab(ui->tabBarConnections);
	ui->tabBarConnections->insertTab(ui->tabBarConnections->count(), connectionTab, tabName);
	ui->tabBarConnections->setCurrentIndex(ui->tabBarConnections->count()-1);
}

void MainWindow::on_action_Open_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, "Open file", "", "");

    //qDebug() << "opened filename: " + filename;
    //qDebug() << QString::number(ui->tabBarConnections->count() > 0);

    /*if (ui->tabBarConnections->count() > 0) {
        int index = ui->tabBarConnections->widget(index);
         QFile
    }*/
}
