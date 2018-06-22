#include "ConnectionManager.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QStandardItemModel>
#include <QSettings>
#include <QSpacerItem>
#include <QGridLayout>
#include <QRandomGenerator64>

ConnectionManager* ConnectionManager::m_instance = 0;

ConnectionManager::ConnectionManager()
{
    m_establishedConnectionModel = new QStandardItemModel();
}

ConnectionManager* ConnectionManager::getInstance()
{
    if(m_instance==0) {
		m_instance = new ConnectionManager();
	}
	return m_instance;
}

QStandardItemModel* ConnectionManager::getEstablishedConnectionModel()
{
    return m_establishedConnectionModel;
}

void ConnectionManager::establishConnection(ConnectionStandardItem *connection)
{
    QRandomGenerator64 random = QRandomGenerator64::securelySeeded();
    qint64 newId = random.generate64();

    QMap<QString, QVariant> connectionDefinition = connection->data(Qt::UserRole+1).value<QMap<QString, QVariant>>();
    connectionDefinition["establishedConnectionId"] = QString("EC_" + QString::number(newId));

    QSqlDatabase db = QSqlDatabase::addDatabase(connectionDefinition["driver"].toString(), connectionDefinition["establishedConnectionId"].toString());

    db.setHostName(connectionDefinition["server"].toString());
    db.setDatabaseName(connectionDefinition["database"].toString());
    db.setUserName(connectionDefinition["username"].toString());
    db.setPassword(connectionDefinition["pass"].toString());
    db.setPort(connectionDefinition["port"].toInt());

	bool ok = db.open();

    if (ok)
    {
        //do nothing?

        QStandardItem* establishedConnection = new QStandardItem();
        establishedConnection->setText(connectionDefinition["name"].toString());
        establishedConnection->setData(connectionDefinition["establishedConnectionId"].toString(), Qt::UserRole+1);

        m_establishedConnectionModel->appendRow(establishedConnection);
    }
    else
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

QStringList ConnectionManager::getEstablishedConnectionList()
{
   return QSqlDatabase::connectionNames();
}
