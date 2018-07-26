#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include <QPlainTextEdit>
#include <QString>
#include <QWidget>
#include <QWidget>
#include <QSqlDatabase>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSplitter>
#include <QComboBox>

namespace Ui {
class ConnectionTab;
}

class ConnectionTab : public QWidget
{
	Q_OBJECT

signals:
   void textChanged();

public:
    explicit ConnectionTab(QString filename, QWidget *parent = 0);
	~ConnectionTab();
    void executeQueryAtCursor(QSqlDatabase sqlDatabase);
    void executeQuery(QSqlDatabase sqlDatabase, QString query);
    QString filename() const;
    void setFilename(const QString &filename);
    bool modified() const;
    void setModified(const bool &modified);
    void readFile();
    void writeFile();

private:
    Ui::ConnectionTab *ui;
    QSqlQueryModel* m_queryResultsModel;
    QString m_filename;
};

#endif // CONNECTIONTAB_H
