#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include "src/Connection.h"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
	Q_OBJECT

public:
    explicit ConnectionDialog(const Connection &connection, QWidget *parent = 0);
    ~ConnectionDialog();
    Connection getConnection() const;

private slots:
    void on_buttonBox_accepted();
	void on_buttonBox_rejected();

    void on_listDropdownDBDriver_currentIndexChanged(int index);
    void on_txtName_textChanged(const QString &arg1);
    void on_txtServer_textChanged(const QString &arg1);
    void on_txtPort_textChanged(const QString &arg1);
    void on_txtDatabase_textChanged(const QString &arg1);
    void on_txtUser_textChanged(const QString &arg1);
    void on_txtPass_textChanged(const QString &arg1);
    void on_chooseDatabaseFileButton_clicked();

private:
    Ui::ConnectionDialog *ui;
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

#endif // CONNECTIONDIALOG_H
