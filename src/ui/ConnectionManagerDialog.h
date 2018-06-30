#ifndef CONNECTIONMANAGERDIALOG_H
#define CONNECTIONMANAGERDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDataWidgetMapper>

#include "core/ConnectionStandardItem.h"

namespace Ui {
class ConnectionManagerDialog;
}

class ConnectionManagerDialog : public QDialog
{
	Q_OBJECT

public:
    explicit ConnectionManagerDialog(QWidget *parent = 0);
    ~ConnectionManagerDialog();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonNewConnection_released();
    void on_listViewSelectionChanged(QItemSelection current, QItemSelection previous);
    void on_rowsInserted(const QModelIndex &source_parent, int start, int end);
    void on_renameListViewItem(QStandardItem* changedItem);

private:
    Ui::ConnectionManagerDialog *ui;

    QStandardItemModel* m_model;
    QStandardItemModel* m_driversModel;
    QStandardItemModel* m_connectionListModel;
    QDataWidgetMapper* m_dataWidgetMapper;
    void updateConnectionListModel();
    void updateCurrentlySelectedConnection();
    QMap<QString, QVariant> getSelectedConnectionDefinition();
    ConnectionStandardItem* getCurrentlySelectedConnection();
};

#endif // CONNECTIONMANAGERDIALOG_H
