#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "ui/ConnectionManagerDialog.h"
#include "ui_ConnectionManagerDialog.h"
#include "src/ConnectionManager.h"

ConnectionManagerDialog::ConnectionManagerDialog(ConnectionManager *connectionManager, QWidget *parent) : m_connectionManager(connectionManager), QDialog(parent), ui(new Ui::ConnectionManagerDialog)
{
	ui->setupUi(this);

    m_driversModel = new QStandardItemModel(this);
    m_connectionListModel = new QStandardItemModel(this);

    ui->listDropdownDBDriver->setModel(m_driversModel);
    ui->listViewConnections->setModel(m_connectionListModel);

    /* set combobox */
    QStandardItem* item_psql = new QStandardItem();
    item_psql->setText("PostgreSQL");
    item_psql->setData("QPSQL");
    m_driversModel->appendRow(item_psql);

    QStandardItem* item_mysql = new QStandardItem();
    item_mysql->setText("MySQL/MariaDB");
    item_mysql->setData("QMYSQL");
    m_driversModel->appendRow(item_mysql);

    populateConnectionListModel();

    connect(ui->listViewConnections->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(on_listViewSelectionChanged(QItemSelection, QItemSelection)));
    connect(m_connectionListModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_renameListViewItem(QStandardItem*)));
    //connect(m_connectionListModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(on_rowsInserted(QModelIndex,int,int)));

    /* disable ui to be edited until we have connection selected/created */
    ui->txtDatabase->setDisabled(true);
    ui->txtPass->setDisabled(true);
    ui->txtPort->setDisabled(true);
    ui->txtServer->setDisabled(true);
    ui->txtUser->setDisabled(true);
    ui->listDropdownDBDriver->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
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
    /*QModelIndex index = m_connectionListModel->index(start, 0);
    if (index.isValid())
    {
        ui->listViewConnections->selectionModel()->clear();
        ui->listViewConnections->selectionModel()->select(index, QItemSelectionModel::Select);
    }*/
}

void ConnectionManagerDialog::on_buttonNewConnection_released()
{
    Connection* newConnection = new Connection();
    m_connectionListModel->appendRow(newConnection);
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

    //QMap<QString, QVariant> connectionDefinition = getSelectedConnectionDefinition();

    //ui->listDropdownDBDriver->setCurrentIndex(index);
    ui->txtUser->setText(getSelectedConnection()->details()["username"]);
    ui->txtPass->setText(getSelectedConnection()->details()["pass"]);
    ui->txtServer->setText(getSelectedConnection()->details()["server"]);
    ui->txtPort->setText(getSelectedConnection()->details()["port"]);
    ui->txtDatabase->setText(getSelectedConnection()->details()["database"]);

    //enable UI to be edited
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    ui->txtDatabase->setEnabled(true);
    ui->txtPass->setEnabled(true);
    ui->txtPort->setEnabled(true);
    ui->txtServer->setEnabled(true);
    ui->txtUser->setEnabled(true);
    ui->listDropdownDBDriver->setEnabled(true);
}

void ConnectionManagerDialog::populateConnectionListModel() {
    m_connectionListModel->clear();
    foreach (Connection connection, m_connectionManager->getConnections().values())
    {
        Connection *connetionPointer = new Connection(connection.connectionId(), connection.driver(), connection.name(), connection.details());
        m_connectionListModel->appendRow(connetionPointer);
    }
}

void ConnectionManagerDialog::updateCurrentlySelectedConnection()
{
    QMap<QString, QString> details;

    details["database"] = ui->txtDatabase->text();
    details["pass"] = ui->txtPass->text();
    details["port"] = ui->txtPort->text();
    details["server"] = ui->txtServer->text();
    details["username"] = ui->txtUser->text();

    Connection *selectedConnection = getSelectedConnection();

    selectedConnection->setDetails(details);
    selectedConnection->setDriver(ui->listDropdownDBDriver->currentData(Qt::UserRole+1).toString());
    selectedConnection->setName(selectedConnection->text());
    m_connectionManager->saveConnection(*selectedConnection);
}

Connection* ConnectionManagerDialog::getSelectedConnection()
{
    QStandardItem* selectedConnection = m_connectionListModel->itemFromIndex(ui->listViewConnections->currentIndex());

    return (Connection*) selectedConnection;
}

void ConnectionManagerDialog::on_pushButton_released()
{
    Connection *connection = getSelectedConnection();

    m_connectionManager->closeConnection(connection->connectionId());
    m_connectionManager->deleteConnection(connection->connectionId());

    populateConnectionListModel();
}
