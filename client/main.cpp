#include <QApplication>
#include "coreclient.h"
#include "clientmainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ClientMainWindow mainWindow;
	mainWindow.show();
	CoreClient cc;

	return a.exec();
}
