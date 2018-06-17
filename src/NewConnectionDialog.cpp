#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "NewConnectionDialog.h"
#include "ui_NewConnectionDialog.h"
#include "Connection.h"

NewConnectionDialog::NewConnectionDialog(QWidget *parent) :	QDialog(parent), ui(new Ui::NewConnectionDialog)
{
	ui->setupUi(this);
	//m_connnectionId = getRandomString();

	m_model = new QStandardItemModel(this);

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

	m_drivers.insert("QPSQL", "PostgreSQL");
    m_drivers.insert("QMYSQL", "MySQL");
    /*m_drivers.insert("QDB2", "DB2");*/

	for (auto key: m_drivers.keys())
	{
		ui->listDropdownDBDriver->addItem(m_drivers.value(key), key);
	}

    ui->listDropdownDBDriver->setCurrentIndex(1);
    /*
     *  insert here some autopopluate code for new connection mask
    */
}

NewConnectionDialog::~NewConnectionDialog()
{
	delete ui;
}

void NewConnectionDialog::on_buttonBox_accepted()
{
	QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole).toString();
	m_connection = new Connection(driver, ui->txtUser->text(), ui->txtPass->text(), ui->txtServer->text(), ui->txtPort->text().toInt(), ui->txtDatabase->text(), "","");

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

Connection* NewConnectionDialog::getConnection()
{
	return m_connection;
}
