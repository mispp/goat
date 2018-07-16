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

private:
    Ui::ConnectionDialog *ui;
    QStandardItemModel m_driversModel;
    Connection m_connection;
};

#endif // CONNECTIONDIALOG_H
