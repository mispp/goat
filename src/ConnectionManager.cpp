#include "ConnectionManager.h"

#include <QApplication>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>
#include <QUuid>
#include <QSqlQuery>
#include <QAction>

#include "StringUtils.h"

ConnectionManager::ConnectionManager() : QObject()
{
    foreach(Connection connection, loadConnections())
    {
        m_connections[connection.connectionId()] = connection;
    }
}

ConnectionManager::~ConnectionManager()
{
    foreach(Connection connection, m_connections.values())
    {
        closeConnection(connection.connectionId());
    }
    m_connections.clear();
}

void ConnectionManager::openConnection(const Connection &connection)
{
    if (QSqlDatabase::contains(connection.connectionId()))
        return;

    QSqlDatabase db = QSqlDatabase::addDatabase(connection.driver(), connection.connectionId());

    if (connection.driver() == "QODBC")
    {
        QMap<QString, QString> values;

        foreach (QString key, connection.details().keys())
        {
            values[key] = connection.details()[key];
            values["escaped-" + key] = connection.details()[key].replace("}", "}}");
        }

        QString databaseName = StringUtils::interpolate(values["connection"], values);
        db.setDatabaseName(databaseName);
        db.setConnectOptions(connection.details()["options"]);
    }
    else
    {
        db.setHostName(connection.details()["server"]);
        db.setPort(connection.details()["port"].toInt());
        db.setDatabaseName(connection.details()["database"]);
        db.setConnectOptions(connection.details()["options"]);
        db.setUserName(connection.details()["username"]);
        db.setPassword(connection.details()["pass"]);
    }

	bool ok = db.open();

    if (!ok)
    {
        QSqlDatabase::removeDatabase(connection.connectionId());

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

    emit connectionStateChanged();
}

void ConnectionManager::closeConnection(const QString &connectionId)
{
    if (!QSqlDatabase::contains(connectionId))
        return;

    QSqlDatabase::database(connectionId).close();
    QSqlDatabase::removeDatabase(connectionId);

    emit connectionStateChanged();
}

void ConnectionManager::saveConnection(const Connection &connection)
{
    if (m_connections.contains(connection.connectionId()))
        closeConnection(connection.connectionId());

    m_connections[connection.connectionId()] = connection;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "connections");

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

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "connections");
    settings.remove(connectionId);
    settings.sync();

    emit connectionDeleted(connectionId);
}

bool ConnectionManager::isOpen(const QString &connectionId) const
{
    return QSqlDatabase::contains(connectionId);
}

QSqlDatabase ConnectionManager::getOpenConnection(const QString &connectionId)
{
    if (!isOpen(connectionId))
        qDebug() << "Call to ConnectionManager::getOpenConnection() was make with non-open connectionId " << connectionId;

    return QSqlDatabase::database(connectionId);
}

QMap<QString, QString> ConnectionManager::getOpenConnections()
{
    QMap<QString, QString> openConnections;

    foreach (QString key, QSqlDatabase::connectionNames())
    {
        if (!key.startsWith("CLONED_"))
        {
            Connection connection = m_connections[key];
            openConnections[key] = connection.name();
        }
    }

    return openConnections;
}

QMap<QString, Connection> ConnectionManager::getConnections() const
{
    return m_connections;
}

QList<Connection> ConnectionManager::loadConnections()
{
    QList<Connection> connections;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "connections");

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
