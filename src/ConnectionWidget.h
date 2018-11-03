#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include "src/Connection.h"

#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
	Q_OBJECT

public:
    explicit ConnectionWidget(QWidget *parent = 0);
    ~ConnectionWidget();
    Connection getConnection() const;
    void setConnection(const Connection &connection);

private slots:
    void on_listDropdownDBDriver_currentIndexChanged(int index);
    void on_txtName_textChanged(const QString &arg1);
    void on_txtServer_textChanged(const QString &arg1);
    void on_txtPort_textChanged(const QString &arg1);
    void on_txtDatabase_textChanged(const QString &arg1);
    void on_txtUser_textChanged(const QString &arg1);
    void on_txtPass_textChanged(const QString &arg1);
    void on_chooseDatabaseFileButton_clicked();
    void on_optionDocumentionButton_clicked();

private:
    Ui::ConnectionWidget *ui;
    QStandardItemModel m_driversModel;
    Connection m_connection;
    bool ignoreChanges;

    Connection buildConnection();
    void setUiValues();
    void setUiValues(const Connection &connection);
    QString driver();
    QString getUiDriver();
    void updateConnection();
};

#endif // CONNECTIONWIDGET_H
