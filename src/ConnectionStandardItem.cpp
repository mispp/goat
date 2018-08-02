#include "ConnectionStandardItem.h"

#include <QSettings>

ConnectionStandardItem::ConnectionStandardItem(QString const& connectionId) : QStandardItem()
{   
    QString connectionIdTrimmed =  connectionId.trimmed();
    QMap<QString, QVariant>  m_connectionDefinition;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    if (!connectionIdTrimmed.isEmpty())
    {
        if (settings.childGroups().contains(connectionIdTrimmed))
        {
            settings.beginGroup(connectionIdTrimmed);

            m_connectionDefinition["connectionId"] = QVariant(connectionIdTrimmed);

            foreach(QString key, settings.childKeys())
            {
                m_connectionDefinition[key] = settings.value(key);
            }

            setText(m_connectionDefinition["name"].toString());
            setData(m_connectionDefinition, Qt::UserRole+1);

            settings.endGroup();
        }
        else
        {
            settings.beginGroup(connectionIdTrimmed);

            m_connectionDefinition["database"] = QVariant("");
            m_connectionDefinition["driver"] = QVariant("PSQL");
            m_connectionDefinition["pass"] = QVariant("");
            m_connectionDefinition["port"] = QVariant("");
            m_connectionDefinition["server"] = QVariant("");
            m_connectionDefinition["username"] = QVariant("");
            m_connectionDefinition["name"] = QVariant("New connection");
            m_connectionDefinition["connectionId"] = connectionIdTrimmed;

            setText(m_connectionDefinition["name"].toString());
            setData(m_connectionDefinition, Qt::UserRole+1);

            settings.endGroup();
        }
    }
}

void ConnectionStandardItem::updateConnectionDefinition(QMap<QString, QVariant> formEntries)
{
    QMap<QString, QVariant> map = data().value<QMap<QString, QVariant>>();

    foreach (QString key, formEntries.keys())
    {
        map[key] = formEntries[key];
    }

    setData(map, Qt::UserRole+1);
    updateConnectionDefinitionOnDisk();
}

void ConnectionStandardItem::updateConnectionDefinitionOnDisk()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    QMap<QString, QVariant> map = data().value<QMap<QString, QVariant>>();

    if (!map["connectionId"].toString().isEmpty())
    {
        settings.beginGroup(map["connectionId"].toString());

        foreach(QString key, map.keys())
        {
            settings.setValue(key, map[key]);
        }

        settings.endGroup();
    }
}
