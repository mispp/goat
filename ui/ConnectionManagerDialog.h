#ifndef CONNECTIONMANAGERDIALOG_H
#define CONNECTIONMANAGERDIALOG_H

#include "src/ConnectionManager.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDataWidgetMapper>

namespace Ui {
class ConnectionManagerDialog;
}

class ConnectionManagerDialog : public QDialog
{
	Q_OBJECT

public:
    explicit ConnectionManagerDialog(ConnectionManager *connectionManager, QWidget *parent = 0);
    ~ConnectionManagerDialog();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonNewConnection_released();
    void on_listViewSelectionChanged(QItemSelection current, QItemSelection previous);
    void on_rowsInserted(const QModelIndex &source_parent, int start, int end);
    void on_renameListViewItem(QStandardItem* changedItem);

    void on_button_deleteConnection_released();

private:
    Ui::ConnectionManagerDialog *ui;

    QStandardItemModel m_driversModel;
    QStandardItemModel m_connectionListModel;
    void populateConnectionListModel();
    void updateCurrentlySelectedConnection();
    QMap<QString, QVariant> getSelectedConnectionDefinition();
    ConnectionManager* m_connectionManager;
    QMap<QString, Connection> m_connectionList;
    Connection* getSelectedConnection();

    void disableEditing();
    void enableEditing();
};

#endif // CONNECTIONMANAGERDIALOG_H
