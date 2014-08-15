#include "clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent) :
	QMainWindow(parent),
	stopPB(tr("Отключить удаленное подключение"), this)
{
	connect(&stopPB, SIGNAL(clicked()), this, SLOT(close()));
	setCentralWidget(&stopPB);
}

