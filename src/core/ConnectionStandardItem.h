#ifndef CONNECTIONSTANDARDITEM_H
#define CONNECTIONSTANDARDITEM_H

#include <QStandardItem>

class ConnectionStandardItem : public QStandardItem
{
public:
    explicit ConnectionStandardItem(QString const& connectionId);
    void updateConnectionDefinition(QMap<QString, QVariant> formEntries);

private:
    void updateConnectionDefinitionOnDisk();

};

Q_DECLARE_METATYPE(ConnectionStandardItem*)

#endif // CONNECTIONSTANDARDITEM_H
