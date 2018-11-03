#include "src/MainWindow.h"
#include "src/Row.h"
#include "src/RowSet.h"
#include "src/Connection.h"
#include "src/Csv.h"

#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Row>("Row");
    qRegisterMetaType<RowSet>("RowSet");
    qRegisterMetaType<Connection>("Connection");
    qRegisterMetaType<Csv>("Csv");

	QApplication a(argc, argv);
	MainWindow w;

    w.show();

	return a.exec();
}
