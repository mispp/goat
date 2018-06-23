#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QRandomGenerator64>

#include "ConnectionManagerDialog.h"
#include "ui_ConnectionManagerDialog.h"
#include "ConnectionManager.h"

ConnectionManagerDialog::ConnectionManagerDialog(QWidget *parent) :	QDialog(parent), ui(new Ui::ConnectionManagerDialog) {
	ui->setupUi(this);

    m_model = new QStandardItemModel(this);
    m_driversModel = new QStandardItemModel(this);
    m_connectionListModel = new QStandardItemModel(this);
    m_dataWidgetMapper = new QDataWidgetMapper(this);
    m_dataWidgetMapper->setModel(m_connectionListModel);

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

    updateConnectionListModel();

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
    /* TODO: get this from model instead of GUI */
    //QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole).toString();
    //const QModelIndex index = ui->listViewConnections->currentIndex();
    //Connection* connection = m_connectionListModel->itemFromIndex(index)->data(Qt::UserRole).value<Connection*>();

    //ConnectionManager::getInstance()->establishConnection(connection);

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
        ConnectionManager::getInstance()->establishConnection(getCurrentlySelectedConnection());
		accept();
	}
	else
	{
		reject();
	}
}

void ConnectionManagerDialog::on_rowsInserted(const QModelIndex &source_parent, int start, int end)
{
    QModelIndex index = m_connectionListModel->index(start, 0);
    if (index.isValid())
    {
        ui->listViewConnections->selectionModel()->clear();
        ui->listViewConnections->selectionModel()->select(index, QItemSelectionModel::Select);
    }
}

void ConnectionManagerDialog::on_buttonNewConnection_released()
{
    QRandomGenerator64 random = QRandomGenerator64::securelySeeded();
    qint64 newId = random.generate64();
    ConnectionStandardItem* item = new ConnectionStandardItem(QString::number(newId));

    m_connectionListModel->appendRow(item);
}

void ConnectionManagerDialog::on_renameListViewItem(QStandardItem* changedItem)
{
    QMap<QString, QVariant> connectionDefinition = changedItem->data(Qt::UserRole+1).value<QMap<QString, QVariant>>();
    QString newName = changedItem->text();
    connectionDefinition["name"] = QVariant(newName);

    getCurrentlySelectedConnection()->updateConnectionDefinition(connectionDefinition);
}

void ConnectionManagerDialog::on_listViewSelectionChanged(QItemSelection current, QItemSelection previous) {
    QMap<QString, QVariant> connectionDefinition = getSelectedConnectionDefinition();

    //ui->listDropdownDBDriver->setCurrentIndex(index);
    ui->txtUser->setText(connectionDefinition["username"].toString());
    ui->txtPass->setText(connectionDefinition["pass"].toString());
    ui->txtServer->setText(connectionDefinition["server"].toString());
    ui->txtPort->setText(connectionDefinition["port"].toString());
    ui->txtDatabase->setText(connectionDefinition["database"].toString());

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

void ConnectionManagerDialog::updateConnectionListModel() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "goat", "connections");

    foreach(QString key, settings.childGroups())
    {
        ConnectionStandardItem* item = new ConnectionStandardItem(key);
        m_connectionListModel->appendRow(item);
    }
}

void ConnectionManagerDialog::updateCurrentlySelectedConnection()
{
    QMap<QString, QVariant> connectionDefinition = getSelectedConnectionDefinition();

    connectionDefinition["database"] = QVariant(ui->txtDatabase->text());
    connectionDefinition["driver"] = ui->listDropdownDBDriver->currentData(Qt::UserRole+1);
    connectionDefinition["pass"] = QVariant(ui->txtPass->text());
    connectionDefinition["port"] = QVariant(ui->txtPort->text().toInt());
    connectionDefinition["server"] = QVariant(ui->txtServer->text());
    connectionDefinition["username"] = QVariant(ui->txtUser->text());

    getCurrentlySelectedConnection()->updateConnectionDefinition(connectionDefinition);
}

ConnectionStandardItem* ConnectionManagerDialog::getCurrentlySelectedConnection()
{
    QStandardItem* currentlySelectedConnection = m_connectionListModel->itemFromIndex(ui->listViewConnections->currentIndex());

    return (ConnectionStandardItem*) currentlySelectedConnection;
}

QMap<QString, QVariant> ConnectionManagerDialog::getSelectedConnectionDefinition()
{
    return getCurrentlySelectedConnection()->data().value<QMap<QString, QVariant>>();
}
