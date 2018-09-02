#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

#include "ui/ConnectionWidget.h"
#include "ui_ConnectionWidget.h"
#include "src/ConnectionManager.h"

ConnectionWidget::ConnectionWidget(QWidget *parent) :	QWidget(parent), ui(new Ui::ConnectionWidget)
{
	ui->setupUi(this);
    this->ignoreChanges = true;
    ui->listDropdownDBDriver->setModel(&m_driversModel);
    m_connection = Connection::defaultConnection();

    QMap<QString, QString> drivers;
    drivers["PostgreSQL"] = "QPSQL";
    drivers["MySQL/MariaDB"] = "QMYSQL";
    drivers["ODBC/MS Sql Server"] = "QODBC";
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

ConnectionWidget::~ConnectionWidget() {
	delete ui;
}

void ConnectionWidget::setUiValues(const Connection &connection)
{
    int idx = ui->listDropdownDBDriver->findData(connection.driver(), Qt::UserRole + 1);
    ui->listDropdownDBDriver->setCurrentIndex(idx);
    ui->txtName->setText(connection.name());
    ui->txtServer->setText(connection.details()["server"]);
    ui->txtPort->setText(connection.details()["port"]);
    ui->txtDatabase->setText(connection.details()["database"]);
    ui->txtUser->setText(connection.details()["username"]);
    ui->txtPass->setText(connection.details()["pass"]);
    ui->txtOptions->setText(connection.details()["options"]);
    ui->txtConnection->setText(connection.details()["connection"]);
    ui->txtLibrary->setText(connection.details()["library"]);
    ui->txtSetup->setText(connection.details()["setup"]);

    ui->chooseDatabaseFileButton->setDisabled(connection.driver() != "QSQLITE");
    ui->txtServer->setDisabled(connection.driver() == "QSQLITE");
    ui->txtPort->setDisabled(connection.driver() == "QSQLITE");
    ui->txtUser->setDisabled(connection.driver() == "QSQLITE");
    ui->txtPass->setDisabled(connection.driver() == "QSQLITE");

    ui->txtConnection->setDisabled(connection.driver() != "QODBC");
    ui->txtLibrary->setDisabled(connection.driver() != "QODBC");
    ui->txtSetup->setDisabled(connection.driver() != "QODBC");
}

Connection ConnectionWidget::buildConnection()
{
    QString driver = ui->listDropdownDBDriver->currentData(Qt::UserRole + 1).toString();

    QMap<QString, QString> details;
    details["server"] = ui->txtServer->text();
    details["port"] = ui->txtPort->text();
    details["database"] = ui->txtDatabase->text();
    details["username"] = ui->txtUser->text();
    details["pass"] = ui->txtPass->text();
    details["options"] = ui->txtOptions->text();

    details["connection"] = ui->txtConnection->text();
    details["library"] = ui->txtLibrary->text();
    details["setup"] = ui->txtSetup->text();

    Connection connection(m_connection.connectionId(), driver, ui->txtName->text(), details);
    return connection;
}

Connection ConnectionWidget::getConnection() const
{
    return m_connection;
}

void ConnectionWidget::on_listDropdownDBDriver_currentIndexChanged(int)
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

        if (connection.driver() == "QSQLITE")
        {
            details.remove("server");
            details.remove("port");
            details.remove("username");
            details.remove("pass");
        }

        if (connection.driver() != "QODBC")
        {
            details.remove("connection");
            details.remove("library");
            details.remove("setup");
        }
    }

    connection.setDetails(details);

    if (hasDefaultName)
        connection.setName(Connection::defaultName(connection));

    setUiValues(connection);
    m_connection = connection;
}

void ConnectionWidget::updateConnection()
{
    if (this->ignoreChanges)
        return;

    if (Connection::defaultName(m_connection) == m_connection.name())
    {
        ui->txtName->setText(Connection::defaultName(buildConnection()));
    }
    m_connection = buildConnection();
}

void ConnectionWidget::on_txtName_textChanged(const QString &)
{
    m_connection.setName(ui->txtName->text());
}

void ConnectionWidget::on_txtServer_textChanged(const QString &)
{
    updateConnection();
}

void ConnectionWidget::on_txtPort_textChanged(const QString &)
{
    updateConnection();
}

void ConnectionWidget::on_txtDatabase_textChanged(const QString &)
{
    updateConnection();
}

void ConnectionWidget::on_txtUser_textChanged(const QString &)
{
    updateConnection();
}

void ConnectionWidget::on_txtPass_textChanged(const QString &)
{
    updateConnection();
}

void ConnectionWidget::on_chooseDatabaseFileButton_clicked()
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

void ConnectionWidget::setConnection(const Connection &connection)
{
    ignoreChanges = true;
    m_connection = connection;
    setUiValues(m_connection);
    ignoreChanges = false;
}

void ConnectionWidget::on_optionDocumentionButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions", QUrl::TolerantMode));
}
