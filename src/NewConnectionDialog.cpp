#include "NewConnectionDialog.h"
#include "ui_NewConnectionDialog.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include "Connection.h"

NewConnectionDialog::NewConnectionDialog(QWidget *parent) :	QDialog(parent), ui(new Ui::NewConnectionDialog)
{
	ui->setupUi(this);
	//m_connnectionId = getRandomString();

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

	for (int i=0; i<settings.childGroups().count(); ++i)
	{
		QString groupName = settings.childGroups()[i];

		settings.beginGroup(groupName);

		Connection *connection = new Connection(
					 settings.value("driver").toString()
					,settings.value("user").toString()
					,settings.value("pass").toString()
					,settings.value("server").toString()
					,settings.value("port").toInt()
					,settings.value("database").toString()
					,settings.value("name").toString()
					,groupName
					);

		QVariant itemData = QVariant::fromValue(connection);

		QListWidgetItem *item = new QListWidgetItem(ui->listWidgetConnections);
		item->setText(connection->getName());
		item->setData(Qt::UserRole, itemData);

		ui->listWidgetConnections->addItem(item);

		settings.endGroup();
	}
}

NewConnectionDialog::~NewConnectionDialog()
{
	delete ui;
}

void NewConnectionDialog::on_buttonBox_accepted()
{
	m_connection = new Connection("QMYSQL", ui->txtUser->text(), ui->txtPass->text(), ui->txtServer->text(), ui->txtPort->text().toInt(), ui->txtDatabase->text(), "","");

	accept();
}

void NewConnectionDialog::on_buttonBox_rejected()
{
	m_connection = NULL;

	reject();
}

QString NewConnectionDialog::getConnectionId()
{
	return m_connnectionId;
}


void NewConnectionDialog::saveConnections()
{

}

void NewConnectionDialog::on_buttonBox_clicked(QAbstractButton *button)
{
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

void NewConnectionDialog::on_listWidgetConnections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	Connection *connection = qvariant_cast<Connection*>(current->data(Qt::UserRole));

	//ui->listDropdownDBDriver->setCurrentIndex();
	ui->txtUser->setText(connection->getUser());
	ui->txtPass->setText(connection->getPass());
	ui->txtServer->setText(connection->getServer());
	ui->txtPort->setText(QString::number(connection->getPort()));
	ui->txtDatabase->setText(connection->getDatabase());
}

Connection* NewConnectionDialog::getConnection()
{
	return m_connection;
}
