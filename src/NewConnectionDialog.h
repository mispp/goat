#ifndef NEWCONNECTIONDIALOG_H
#define NEWCONNECTIONDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDataWidgetMapper>

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
    //void on_listWidgetConnections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_buttonNewConnection_released();
    void handleSelectionChanged(QItemSelection current, QItemSelection previous);
    void updateListViewItem(QStandardItem* changedItem);

private:
	Ui::NewConnectionDialog *ui;

    Connection* m_connection;

    QStandardItemModel* m_model;
    QStandardItemModel* m_driversModel;
    QStandardItemModel* m_connectionListModel;
    QDataWidgetMapper* m_dataWidgetMapper;
    void updateConnectionListModel();
    void updateCurrentlySelectedConnection();
};

#endif // NEWCONNECTIONDIALOG_H
