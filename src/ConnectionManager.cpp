#include "ConnectionManager.h"
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>
#include <QDebug>
#include <QSqlError>
#include <QStandardItemModel>

ConnectionManager* ConnectionManager::m_instance = 0;

ConnectionManager::ConnectionManager()
{
	m_model = new QStandardItemModel();
}

ConnectionManager* ConnectionManager::getInstance()
{
	if(m_instance==0)
	{
		m_instance = new ConnectionManager();
	}
	return m_instance;
}


void ConnectionManager::addConnection(Connection *connection)
{
	QSqlDatabase db = QSqlDatabase::addDatabase(connection->getDriver(), connection->getConnectionId());

	db.setHostName(connection->getServer());
	db.setDatabaseName(connection->getDatabase());
	db.setUserName(connection->getUser());
	db.setPassword(connection->getPass());
	db.setPort(connection->getPort());

	bool ok = db.open();

	qDebug() << "Number of currently opened connections: " + QString::number(db.connectionNames().count());

	if (ok == true)
	{
		qDebug() << "Connection opened, id is " + connection->getConnectionId();

		m_connectionsList.append(connection);

		if (m_connectionsList.count() == 1)
		{
			//set default connection somehow
		}

		updateModel(connection);

	} else
	{
		qDebug() << "Error making connection";

		QString e1 = db.lastError().driverText() + "\n\n"
				   + db.lastError().databaseText() + "\n\n"
				   + "Native error code: " + db.lastError().nativeErrorCode();

		QMessageBox errorCreatingDBConnectionDialog;
		errorCreatingDBConnectionDialog.setWindowTitle("Error");
		errorCreatingDBConnectionDialog.setText("Error creating DB connection?");
		errorCreatingDBConnectionDialog.setInformativeText(e1);
		errorCreatingDBConnectionDialog.setIcon(QMessageBox::Critical);
		errorCreatingDBConnectionDialog.setStandardButtons(QMessageBox::Ok);
		errorCreatingDBConnectionDialog.setMinimumSize(QSize(600, 120));
		QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		QGridLayout* layout = (QGridLayout*)errorCreatingDBConnectionDialog.layout();
		layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

		errorCreatingDBConnectionDialog.exec();
	}

}

bool ConnectionManager::connectionAvailable(Connection* connection)
{
	return m_connectionsList.contains(connection);
}

QStandardItemModel* ConnectionManager::getModel()
{
	return m_model;
}

void ConnectionManager::updateModel(Connection* connection)
{
	QStandardItem* item = new QStandardItem();
	item->setText(connection->getName());
	item->setData(connection->getConnectionId(), Qt::UserRole);

	m_model->appendRow(item);
}

Connection* ConnectionManager::getDefaultConnection()
{
	if (m_connectionsList.count() > 0)
	{
		return m_connectionsList[0];
	} else
	{
		return NULL;
	}
}
