#ifndef NEWCONNECTIONDIALOG_H
#define NEWCONNECTIONDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QAbstractButton>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDataWidgetMapper>

#include "ConnectionStandardItem.h"

namespace Ui {
class NewConnectionDialog;
}

class NewConnectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewConnectionDialog(QWidget *parent = 0);
	~NewConnectionDialog();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonNewConnection_released();
    void on_listViewSelectionChanged(QItemSelection current, QItemSelection previous);
    void on_rowsInserted(const QModelIndex &source_parent, int start, int end);
    void on_renameListViewItem(QStandardItem* changedItem);

private:
	Ui::NewConnectionDialog *ui;

    QStandardItemModel* m_model;
    QStandardItemModel* m_driversModel;
    QStandardItemModel* m_connectionListModel;
    QDataWidgetMapper* m_dataWidgetMapper;
    void updateConnectionListModel();
    void updateCurrentlySelectedConnection();
    QMap<QString, QVariant> getSelectedConnectionDefinition();
    ConnectionStandardItem* getCurrentlySelectedConnection();
};

#endif // NEWCONNECTIONDIALOG_H
