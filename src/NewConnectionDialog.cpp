#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "NewConnectionDialog.h"
#include "ui_NewConnectionDialog.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include "StaticFunctions.h"

NewConnectionDialog::NewConnectionDialog(QWidget *parent) :	QDialog(parent), ui(new Ui::NewConnectionDialog) {
	ui->setupUi(this);

    m_model = new QStandardItemModel(this);
    m_driversModel = new QStandardItemModel(this);
    m_connectionListModel = new QStandardItemModel(this);
    m_dataWidgetMapper = new QDataWidgetMapper(this);
    m_dataWidgetMapper->setModel(m_connectionListModel);

    ui->listDropdownDBDriver->setModel(m_driversModel);
    ui->listViewConnections->setModel(m_connectionListModel);

    /* set connections */
    /*QMap<QString, QStringList> connections = ConnectionManager::getSavedConnections();

    int counter = 0;
    foreach (QString key, connections.keys()) {
        QList<QStandardItem*> items;
        foreach (QString value, connections[key]) {
            items.append(new QStandardItem(value));
        }
        m_connectionListModel->appendColumn(items);

        if (key == "database") m_dataWidgetMapper->addMapping(ui->txtDatabase, counter);
        else if (key == "pass") m_dataWidgetMapper->addMapping(ui->txtPass, counter);
        else if (key == "port") m_dataWidgetMapper->addMapping(ui->txtPort, counter);
        else if (key == "server") m_dataWidgetMapper->addMapping(ui->txtServer, counter);
        else if (key == "username") m_dataWidgetMapper->addMapping(ui->txtUser, counter);
        ++counter;
    }
    */

    /* set combobox */
    QStandardItem* item_psql = new QStandardItem();
    item_psql->setText("PostgreSQL");
    item_psql->setData("QPSQL", Qt::UserRole);
    m_driversModel->appendRow(item_psql);

    QStandardItem* item_mysql = new QStandardItem();
    item_mysql->setText("MySQL/MariaDB");
    item_mysql->setData("QMYSQL", Qt::UserRole);
    m_driversModel->appendRow(item_mysql);

    updateConnectionListModel();

    connect(ui->listViewConnections->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(handleSelectionChanged(QItemSelection, QItemSelection)));
    connect(m_connectionListModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateListViewItem(QStandardItem*)));


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

NewConnectionDialog::~NewConnectionDialog() {
	delete ui;
}

void NewConnectionDialog::on_buttonBox_accepted() {
    /* TODO: get this from model instead of GUI */
    QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole).toString();
    Connection *connection = new Connection(driver, ui->txtUser->text(), ui->txtPass->text(), ui->txtServer->text(), ui->txtPort->text().toInt(), ui->txtDatabase->text(), "","");
    ConnectionManager::getInstance()->establishConnection(connection);

	accept();
}

void NewConnectionDialog::on_buttonBox_rejected() {
	m_connection = NULL;

	reject();
}

QString NewConnectionDialog::getConnectionId() {
    return m_connection->getConnectionId();
}


void NewConnectionDialog::on_buttonBox_clicked(QAbstractButton *button) {
	if (button == ui->buttonBox->button(QDialogButtonBox::Apply))
	{
        //save, but not connect
        updateCurrentlySelectedConnection();
	}
	else if (button == ui->buttonBox->button(QDialogButtonBox::Ok))
	{
        //save, then connect
        updateCurrentlySelectedConnection();
		accept();
	}
	else
	{
		reject();
	}
}

Connection* NewConnectionDialog::getConnection() {
	return m_connection;
}

void NewConnectionDialog::on_buttonNewConnection_released()
{
    Connection* connection = new Connection(
                 QString("QPSQL") //driver
                ,QString() //user
                ,QString() //pass
                ,QString("127.0.0.1") //server
                ,5432 //port
                ,QString() //database
                ,QString("New connection") //connectionname
                ,StaticFunctions::getRandomString() //connectionid
    );

    connection->persistConnection();

    QVariant connectionData = QVariant::fromValue(connection);
    QStandardItem *item = new QStandardItem();
    item->setText(connection->getName());
    item->setData(connectionData, Qt::UserRole);

    m_connectionListModel->appendRow(item);
}

void NewConnectionDialog::updateConnectionListModel() {
    foreach (Connection* connection, ConnectionManager::loadSavedConnections()) {
        QVariant connectionData = QVariant::fromValue(connection);
        QStandardItem *item = new QStandardItem();
        item->setText(connection->getName());
        item->setData(connectionData, Qt::UserRole);

        m_connectionListModel->appendRow(item);
    }
}

void NewConnectionDialog::handleSelectionChanged(QItemSelection current, QItemSelection previous) {
    Connection *connection = m_connectionListModel->data(current.indexes().first(), Qt::UserRole).value<Connection*>();

    //ui->listDropdownDBDriver->setCurrentIndex(index);
    ui->txtUser->setText(connection->getUser());
    ui->txtPass->setText(connection->getPass());
    ui->txtServer->setText(connection->getServer());
    ui->txtPort->setText(QString::number(connection->getPort()));
    ui->txtDatabase->setText(connection->getDatabase());

    /* enable UI to be edited */
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    ui->txtDatabase->setEnabled(true);
    ui->txtPass->setEnabled(true);
    ui->txtPort->setEnabled(true);
    ui->txtServer->setEnabled(true);
    ui->txtUser->setEnabled(true);
    ui->listDropdownDBDriver->setEnabled(true);
}

void NewConnectionDialog::updateListViewItem(QStandardItem* changedItem) {
    Connection* connection = changedItem->data(Qt::UserRole).value<Connection*>();
    connection->setName(changedItem->text());
    connection->persistConnection();
}

void NewConnectionDialog::updateCurrentlySelectedConnection()
{
    const QModelIndex index = ui->listViewConnections->currentIndex();
    Connection* connection = m_connectionListModel->itemFromIndex(index)->data(Qt::UserRole).value<Connection*>();

    connection->setDatabase(ui->txtDatabase->text());
    connection->setDriver(ui->listDropdownDBDriver->currentData().value<QString>());
    connection->setPass(ui->txtPass->text());
    connection->setPort(ui->txtPort->text().toInt());
    connection->setServer(ui->txtServer->text());
    connection->setUser(ui->txtUser->text());

    connection->persistConnection();
}
