#ifndef CONNECTION_H
#define CONNECTION_H

#include<QMap>
#include<QString>

class Connection
{
public:
    static Connection defaultConnection(const QString &driver = "QPSQL");
    static QString defaultName(const Connection &connection);

    Connection();
    Connection(const QString &connectionId, const QString &driver, const QString &name, const QMap<QString, QString> &details);
    QString connectionId() const;
    QString driver() const;
    void setDriver(const QString &driver);
    QString name() const;
    void setName(const QString &name);
    QMap<QString, QString> details() const;
    void setDetails(const QMap<QString, QString> &details);

private:
    QString m_connectionId;
    QString m_driver;
    QString m_name;
    QMap<QString, QString> m_details;
};

//Q_DECLARE_METATYPE(Connection*);

#endif // CONNECTION_H
