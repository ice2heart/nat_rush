#include <QApplication>
#include <QTextCodec>
#include "coreclient.h"
#include "clientmainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
#endif
	ClientMainWindow mainWindow;
	mainWindow.show();
	CoreClient cc;
	QObject::connect(&cc, SIGNAL(connectionInfo(quint8,QString)), &mainWindow, SLOT(info(quint8,QString)));
	return a.exec();
}
