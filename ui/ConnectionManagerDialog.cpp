#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "../ui/ConnectionManagerDialog.h"
#include "ui_ConnectionManagerDialog.h"
#include "../src/ConnectionManager.h"

ConnectionManagerDialog::ConnectionManagerDialog(ConnectionManager *connectionManager, QWidget *parent) : QDialog(parent), ui(new Ui::ConnectionManagerDialog), m_connectionManager(connectionManager)
{
	ui->setupUi(this);
    ui->listViewConnections->setModel(&m_connectionListModel);

    populateConnectionListModel();

    connect(ui->listViewConnections->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(on_listViewSelectionChanged(QItemSelection, QItemSelection)));
    connect(&m_connectionListModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_renameListViewItem(QStandardItem*)));
    connect(&m_connectionListModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(on_rowsInserted(QModelIndex,int,int)));

    /* disable ui to be edited until we have connection selected/created */
    disableEditing();
}

ConnectionManagerDialog::~ConnectionManagerDialog() {
	delete ui;
}

void ConnectionManagerDialog::on_buttonBox_accepted() {
	accept();
}

void ConnectionManagerDialog::on_buttonBox_rejected() {
	reject();
}

void ConnectionManagerDialog::on_buttonBox_clicked(QAbstractButton *button) {

    if (button == ui->buttonBox->button(QDialogButtonBox::Apply))
	{
        updateCurrentlySelectedConnection();
	}
	else if (button == ui->buttonBox->button(QDialogButtonBox::Ok))
	{
        updateCurrentlySelectedConnection();
        m_connectionManager->openConnection(*getSelectedConnection());
		accept();
	}
	else
	{
		reject();
    }
}

void ConnectionManagerDialog::on_rowsInserted(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(source_parent);

    if (start >= 0 && end == start)
    {
        QModelIndex index = m_connectionListModel.item(start)->index();

        if (index.isValid()) ui->listViewConnections->setCurrentIndex(index);
    }
}

void ConnectionManagerDialog::on_buttonNewConnection_released()
{
    Connection* newConnection = new Connection(Connection::defaultConnection());

    m_connectionListModel.appendRow(newConnection);
    m_connectionManager->saveConnection(*newConnection);
}

void ConnectionManagerDialog::on_renameListViewItem(QStandardItem* changedItem)
{
    Connection *changedConnection = (Connection*) changedItem;
    changedConnection->setName(changedItem->text());
    m_connectionManager->saveConnection(*changedConnection);
}

void ConnectionManagerDialog::on_listViewSelectionChanged(QItemSelection current, QItemSelection previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);

    ui->connectionWidget->setConnection(*getSelectedConnection());

    //enable UI to be edited
    enableEditing();
}

void ConnectionManagerDialog::populateConnectionListModel() {
    m_connectionListModel.clear();
    foreach (Connection connection, m_connectionManager->getConnections().values())
    {
        Connection *connetionPointer = new Connection(connection.connectionId(), connection.driver(), connection.name(), connection.details());
        m_connectionListModel.appendRow(connetionPointer);
    }

    m_connectionListModel.sort(0);
}

void ConnectionManagerDialog::updateCurrentlySelectedConnection()
{
    Connection *selectedConnection = getSelectedConnection();
    Connection connection = ui->connectionWidget->getConnection();

    if (selectedConnection != nullptr)
    {
        if (!selectedConnection->connectionId().isEmpty())
        {
            selectedConnection->setDetails(connection.details());
            selectedConnection->setDriver(connection.driver());
            selectedConnection->setName(connection.name());
            m_connectionManager->saveConnection(*selectedConnection);
        }
    }
}

Connection* ConnectionManagerDialog::getSelectedConnection()
{
    QModelIndex index = ui->listViewConnections->currentIndex();

    if (index.isValid())
    {
        QStandardItem* selectedConnection = m_connectionListModel.itemFromIndex(index);

        return (Connection*) selectedConnection;
    }
    else return nullptr;
}

void ConnectionManagerDialog::on_button_deleteConnection_released()
{
    Connection *connection = getSelectedConnection();

    m_connectionManager->closeConnection(connection->connectionId());
    m_connectionManager->deleteConnection(connection->connectionId());

    populateConnectionListModel();

    ui->listViewConnections->clearSelection();
    disableEditing();
}

void ConnectionManagerDialog::disableEditing()
{
    /* disable ui to be edited until we have connection selected/created */
    ui->connectionWidget->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
}

void ConnectionManagerDialog::enableEditing()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    ui->connectionWidget->setEnabled(true);
}

