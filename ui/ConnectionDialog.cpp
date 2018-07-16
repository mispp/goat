#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QUuid>

#include "ui/ConnectionDialog.h"
#include "ui_ConnectionDialog.h"
#include "src/ConnectionManager.h"

ConnectionDialog::ConnectionDialog(const Connection &connection, QWidget *parent) :	QDialog(parent), ui(new Ui::ConnectionDialog)
{
	ui->setupUi(this);
    ui->listDropdownDBDriver->setModel(&m_driversModel);
    this->m_connection = connection;

    /* set combobox */
    QStandardItem* item_psql = new QStandardItem();
    item_psql->setText("PostgreSQL");
    item_psql->setData("QPSQL");
    m_driversModel.appendRow(item_psql);

    QStandardItem* item_mysql = new QStandardItem();
    item_mysql->setText("MySQL/MariaDB");
    item_mysql->setData("QMYSQL");
    m_driversModel.appendRow(item_mysql);

    ui->listDropdownDBDriver->setCurrentIndex(m_connection.driver() == "QPSQL" ? 0 : 1);
    ui->txtName->setText(m_connection.name());
    ui->txtServer->setText(m_connection.details()["server"]);
    ui->txtPort->setText(m_connection.details()["port"]);
    ui->txtDatabase->setText(m_connection.details()["database"]);
    ui->txtUser->setText(m_connection.details()["username"]);
    ui->txtPass->setText(m_connection.details()["pass"]);
}

ConnectionDialog::~ConnectionDialog() {
	delete ui;
}

void ConnectionDialog::on_buttonBox_accepted() {

    QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole+1).toString();

    QMap<QString, QString> details;
    details["server"] = ui->txtServer->text();
    details["port"] = ui->txtPort->text();
    details["database"] = ui->txtDatabase->text();
    details["username"] = ui->txtUser->text();
    details["pass"] = ui->txtPass->text();

    m_connection.setDriver(driver);
    m_connection.setName(ui->txtName->text());
    m_connection.setDetails(details);

	accept();
}

void ConnectionDialog::on_buttonBox_rejected() {
	reject();
}

Connection ConnectionDialog::getConnection() const
{
    return m_connection;
}
