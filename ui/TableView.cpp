#include "TableView.h"
#include "ui_TableView.h"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QKeySequence>

#include "src/Csv.h"

TableView::TableView(QWidget *parent) : QTableView(parent), ui(new Ui::TableView) { ui->setupUi(this); }

TableView::~TableView() { delete ui; }

void TableView::keyPressEvent(QKeyEvent *event)
{
    if (!selectedIndexes().isEmpty())
    {
        if (event->matches(QKeySequence::Copy))
        {
            QHash<QString, QString> parameters;
            parameters["timeFormat"] = "HH:mm:ss";
            parameters["timestampFormat"] = "yyyy-MM-ddTHH:mm:ss";
            parameters["dateFormat"] = "yyyy-MM-dd";

            //DataFormatter(QHash<QString, QString>()
            Csv csv("\t", "\"", false, true, false, parameters);

            QString text = csv.writeSelectionToString(model(), selectionModel()->selection());

            QApplication::clipboard()->setText(text);
        }
        else
            QTableView::keyPressEvent(event);
    }
}
