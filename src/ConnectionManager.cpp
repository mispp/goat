#include "ConnectionManager.h"
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSpacerItem>
#include <QGridLayout>
#include <QDebug>
#include <QSqlError>
#include <QStandardItemModel>
#include <QSettings>

ConnectionManager* ConnectionManager::m_instance = 0;

ConnectionManager::ConnectionManager() {
    m_connectionListModel = new QStandardItemModel();
}

ConnectionManager* ConnectionManager::getInstance() {
    if(m_instance==0) {
		m_instance = new ConnectionManager();
	}
	return m_instance;
}


void ConnectionManager::establishConnection(Connection *connection) {
	QSqlDatabase db = QSqlDatabase::addDatabase(connection->getDriver(), connection->getConnectionId());

	db.setHostName(connection->getServer());
	db.setDatabaseName(connection->getDatabase());
	db.setUserName(connection->getUser());
	db.setPassword(connection->getPass());
	db.setPort(connection->getPort());

	bool ok = db.open();

    if (ok)	{
        updateModel();
        qDebug() << "Updated connection model";
    }
    else {
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

QStandardItemModel* ConnectionManager::getModel() {
    return m_connectionListModel;
}

void ConnectionManager::updateModel() {
    foreach (QString single_db_connection, QSqlDatabase::connectionNames()) {
        if (m_connectionListModel->findItems(single_db_connection).count() == 0) {
            QStandardItem* item = new QStandardItem();

            item->setText(single_db_connection);
            item->setData(single_db_connection, Qt::UserRole);
            m_connectionListModel->appendRow(item);
        }
    }
}

void ConnectionManager::loadSavedConnections() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    for (int i=0; i<settings.childGroups().count(); ++i) {
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

        /*
        QVariant itemData = QVariant::fromValue(connection);

        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetConnections);
        item->setText(connection->getName());
        item->setData(Qt::UserRole, itemData);

        ui->listWidgetConnections->addItem(item);
        */

        settings.endGroup();
    }
}
