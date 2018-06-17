#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "NewConnectionDialog.h"
#include "ui_NewConnectionDialog.h"
#include "Connection.h"
#include "ConnectionManager.h"

NewConnectionDialog::NewConnectionDialog(QWidget *parent) :	QDialog(parent), ui(new Ui::NewConnectionDialog) {
	ui->setupUi(this);

    m_model = new QStandardItemModel(this);
    m_driversModel = new QStandardItemModel(this);
    m_connectionListModel = new QStandardItemModel(this);

    //ConnectionManager::load bla bla

    QStandardItem* item_psql = new QStandardItem();
    item_psql->setText("PostgreSQL");
    item_psql->setData("QPSQL", Qt::UserRole);
    m_driversModel->appendRow(item_psql);

    QStandardItem* item_mysql = new QStandardItem();
    item_mysql->setText("MySQL/MariaDB");
    item_mysql->setData("QMYSQL", Qt::UserRole);
    m_driversModel->appendRow(item_mysql);

    ui->listDropdownDBDriver->setModel(m_driversModel);

    //ui->listDropdownDBDriver->setCurrentIndex(1);
    /*
     *  insert here some autopopluate code for new connection mask
    */
}

NewConnectionDialog::~NewConnectionDialog() {
	delete ui;
}

void NewConnectionDialog::on_buttonBox_accepted() {
    /* TODO: get this from model instead of GUI */
    QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole).toString();
    //QString driver = m_driversModel->
    Connection *connection = new Connection(driver, ui->txtUser->text(), ui->txtPass->text(), ui->txtServer->text(), ui->txtPort->text().toInt(), ui->txtDatabase->text(), "","");

    ConnectionManager::getInstance()->establishConnection(connection);

	accept();
}

void NewConnectionDialog::on_buttonBox_rejected() {
	m_connection = NULL;

	reject();
}

QString NewConnectionDialog::getConnectionId() {
    return m_connection->getConnectionId();
}


void NewConnectionDialog::saveConnections() {

    /*
        move connection i/o to connection manager
    */

    /*qDebug() << "updating connections";

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");
    settings.beginGroup("MainWindow");

    settings.setValue("size", this->size());
    settings.setValue("position", this->pos());
    settings.setValue("connectionBarSize", ui->mainSplitter->sizes()[0]);

    settings.endGroup();*/
}

void NewConnectionDialog::on_buttonBox_clicked(QAbstractButton *button) {
	if (button == ui->buttonBox->button(QDialogButtonBox::Apply))
	{
		qDebug() << "Apply clicked";
	}
	else if (button == ui->buttonBox->button(QDialogButtonBox::Ok))
	{
		accept();
	}
	else
	{
		reject();
	}
}

void NewConnectionDialog::on_listWidgetConnections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
	Connection *connection = qvariant_cast<Connection*>(current->data(Qt::UserRole));

	QString driver = connection->getDriver();

	int index = ui->listDropdownDBDriver->findData(connection->getDriver());
	if (index != -1)
	{
		ui->listDropdownDBDriver->setCurrentIndex(index);
		ui->txtUser->setText(connection->getUser());
		ui->txtPass->setText(connection->getPass());
		ui->txtServer->setText(connection->getServer());
		ui->txtPort->setText(QString::number(connection->getPort()));
		ui->txtDatabase->setText(connection->getDatabase());

		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
	else
	{
		QMessageBox errorDialog;
		errorDialog.setWindowTitle("Configuration problem");
		errorDialog.setText("Driver " + driver + " doesn't exits");
		errorDialog.setInformativeText("It seems that non-existent driver has been referenced in the connections.ini");
		errorDialog.setIcon(QMessageBox::Critical);
		errorDialog.setStandardButtons(QMessageBox::Ok);
		errorDialog.setMinimumSize(QSize(600, 120));
		QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		QGridLayout* layout = (QGridLayout*)errorDialog.layout();
		layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

		errorDialog.exec();

		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}

}

Connection* NewConnectionDialog::getConnection() {
	return m_connection;
}
