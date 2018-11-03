#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "src/DataFormatter.h"
#include "src/Csv.h"

#include <QTableView>
#include <QWidget>

namespace Ui {
class TableView;
}

class TableView : public QTableView
{
    Q_OBJECT

public:
    explicit TableView(QWidget *parent = 0);
    ~TableView();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::TableView *ui;
};

#endif // TABLEVIEW_H
