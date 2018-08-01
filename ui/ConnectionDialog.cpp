#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

#include "ui/ConnectionDialog.h"
#include "ui_ConnectionDialog.h"
#include "src/ConnectionManager.h"

ConnectionDialog::ConnectionDialog(const Connection &connection, QWidget *parent) :	QDialog(parent), ui(new Ui::ConnectionDialog)
{
	ui->setupUi(this);
    this->ignoreChanges = true;
    ui->listDropdownDBDriver->setModel(&m_driversModel);
    this->m_connection = connection;

    QMap<QString, QString> drivers;
    drivers["PostgreSQL"] = "QPSQL";
    drivers["MySQL/MariaDB"] = "QMYSQL";
    drivers["Sqlite"] = "QSQLITE";

    foreach(QString key, drivers.keys())
    {
        QStandardItem* item = new QStandardItem(key);
        item->setData(drivers[key]);
        m_driversModel.appendRow(item);
    }

    setUiValues(m_connection);
    this->ignoreChanges = false;
}

ConnectionDialog::~ConnectionDialog() {
	delete ui;
}

void ConnectionDialog::setUiValues(const Connection &connection)
{
    int idx = ui->listDropdownDBDriver->findData(connection.driver(), Qt::UserRole + 1);
    ui->listDropdownDBDriver->setCurrentIndex(idx);
    ui->chooseDatabaseFileButton->setDisabled(connection.driver() != "QSQLITE");
    ui->txtName->setText(connection.name());
    ui->txtServer->setText(connection.details()["server"]);
    ui->txtPort->setText(connection.details()["port"]);
    ui->txtDatabase->setText(connection.details()["database"]);
    ui->txtUser->setText(connection.details()["username"]);
    ui->txtPass->setText(connection.details()["pass"]);
}

Connection ConnectionDialog::buildConnection()
{
    QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole + 1).toString();

    QMap<QString, QString> details;
    details["server"] = ui->txtServer->text();
    details["port"] = ui->txtPort->text();
    details["database"] = ui->txtDatabase->text();
    details["username"] = ui->txtUser->text();
    details["pass"] = ui->txtPass->text();

    Connection connection(m_connection.connectionId(), driver, ui->txtName->text(), details);
    return connection;
}

void ConnectionDialog::on_buttonBox_accepted() {

    m_connection = buildConnection();
	accept();
}

void ConnectionDialog::on_buttonBox_rejected() {
	reject();
}

Connection ConnectionDialog::getConnection() const
{
    return m_connection;
}

void ConnectionDialog::on_listDropdownDBDriver_currentIndexChanged(int index)
{
    if (this->ignoreChanges)
        return;

    QString oldDriver = m_connection.driver();
    Connection oldDriverConnection = buildConnection();
    oldDriverConnection.setDriver(oldDriver);
    Connection defaultOldDriver = Connection::defaultConnection(oldDriver);
    bool hasDefaultName = oldDriverConnection.name() == Connection::defaultName(oldDriverConnection);

    Connection connection = buildConnection();
    Connection defaultConnection = Connection::defaultConnection(connection.driver());

    QMap<QString, QString> details;

    foreach(QString key, connection.details().keys())
    {
        QString value = connection.details()[key];
        if (value == defaultOldDriver.details()[key])
            details[key] = defaultConnection.details()[key];
        else
            details[key] = value;
    }

    connection.setDetails(details);

    if (hasDefaultName)
        connection.setName(Connection::defaultName(connection));

    setUiValues(connection);
    m_connection = connection;
}

void ConnectionDialog::updateConnection()
{
    if (this->ignoreChanges)
        return;

    if (Connection::defaultName(m_connection) == m_connection.name())
    {
        ui->txtName->setText(Connection::defaultName(buildConnection()));
    }
    m_connection = buildConnection();
}

void ConnectionDialog::on_txtName_textChanged(const QString &arg1)
{
    m_connection.setName(ui->txtName->text());
}

void ConnectionDialog::on_txtServer_textChanged(const QString &arg1)
{
    updateConnection();
}

void ConnectionDialog::on_txtPort_textChanged(const QString &arg1)
{
    updateConnection();
}

void ConnectionDialog::on_txtDatabase_textChanged(const QString &arg1)
{
    updateConnection();
}

void ConnectionDialog::on_txtUser_textChanged(const QString &arg1)
{
    updateConnection();
}

void ConnectionDialog::on_txtPass_textChanged(const QString &arg1)
{
    updateConnection();
}

void ConnectionDialog::on_chooseDatabaseFileButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Sqlite files (*.sqlite3 *.sqlite *.db) ;; All files (*.*)"));
    dialog.setFileMode(QFileDialog::AnyFile); //allow creating a new database
    dialog.setOptions(QFileDialog::DontConfirmOverwrite);
    if (dialog.exec() == QDialog::Accepted && dialog.selectedFiles().count() > 0)
    {
        QString filename = dialog.selectedFiles().at(0);
        if (!filename.isEmpty())
        {
            ui->txtDatabase->setText(filename);
        }
    }
}
