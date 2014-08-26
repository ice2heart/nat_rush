#include <QApplication>
#include "coreclient.h"
#include "clientmainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ClientMainWindow mainWindow;
	mainWindow.show();
	CoreClient cc;
	QObject::connect(&cc, SIGNAL(connectionInfo(quint8,QString)), &mainWindow, SLOT(info(quint8,QString)));
	cc.tryConnect();
	return a.exec();
}
