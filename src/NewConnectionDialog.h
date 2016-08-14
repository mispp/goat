#ifndef NEWCONNECTIONDIALOG_H
#define NEWCONNECTIONDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>

#include "Connection.h"

namespace Ui {
class NewConnectionDialog;
}

class NewConnectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewConnectionDialog(QWidget *parent = 0);
	~NewConnectionDialog();
	QString getConnectionId();
	Connection* getConnection();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_buttonBox_clicked(QAbstractButton *button);

	void on_listWidgetConnections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
	Ui::NewConnectionDialog *ui;
	QString m_connnectionId;
	void saveConnections();
	Connection* m_connection;
	QStandardItemModel* m_model;
	QMap<QString, QString> m_drivers;
};

#endif // NEWCONNECTIONDIALOG_H
