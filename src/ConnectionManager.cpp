#include "ConnectionManager.h"

#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>
#include <QUuid>

ConnectionManager::ConnectionManager()
{
    foreach(Connection connection, loadConnections())
    {
        m_connections[connection.connectionId()] = connection;
    }
}

void ConnectionManager::openConnection(const Connection &connection)
{
    if (m_openConnections.contains(connection.connectionId()))
        return;

    QSqlDatabase db = QSqlDatabase::addDatabase(connection.driver(), connection.connectionId());

    db.setHostName(connection.details()["server"]);
    db.setDatabaseName(connection.details()["database"]);
    db.setUserName(connection.details()["username"]);
    db.setPassword(connection.details()["pass"]);
    db.setPort(connection.details()["port"].toInt());

	bool ok = db.open();

    if (ok)
    {
        m_openConnections[connection.connectionId()] = db;
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

void ConnectionManager::closeConnection(const QString &connectionId)
{
    if (!m_openConnections.contains(connectionId))
        return;

    m_openConnections[connectionId].close();
    m_openConnections.remove(connectionId);
    QSqlDatabase::removeDatabase(connectionId);
}

void ConnectionManager::saveConnection(const Connection &connection)
{
    if (m_connections.contains(connection.connectionId()))
        closeConnection(connection.connectionId());

    m_connections[connection.connectionId()] = connection;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    settings.beginGroup(connection.connectionId());

    settings.setValue("connectionId", connection.connectionId());
    settings.setValue("driver", connection.driver());
    settings.setValue("name", connection.name());

    foreach(QString key, connection.details().keys())
    {
        settings.setValue(key, connection.details()[key]);
    }

    settings.endGroup();
    settings.sync();
}

void ConnectionManager::deleteConnection(const QString &connectionId)
{
    closeConnection(connectionId);
    m_connections.remove(connectionId);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");
    settings.remove(connectionId);
    settings.sync();
}

QMap<QString, QSqlDatabase> ConnectionManager::getOpenConnections() const
{
    return m_openConnections;
}

QMap<QString, Connection> ConnectionManager::getConnections() const
{
    return m_connections;
}

QList<Connection> ConnectionManager::loadConnections()
{
    QList<Connection> connections;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    foreach(QString connectionId, settings.childGroups())
    {
        settings.beginGroup(connectionId);

        QString driver = settings.value("driver").toString();
        QString name = settings.value("name").toString();

        QMap<QString, QString> details;

        foreach(QString key, settings.childKeys())
        {
            details[key] = settings.value(key).toString();
        }
        details.remove("connectionId");
        details.remove("driver");
        details.remove("name");

        Connection connection(connectionId, driver, name, details);
        connections.append(connection);

        settings.endGroup();
    }
    return connections;
}
