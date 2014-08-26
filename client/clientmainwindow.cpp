#include "clientmainwindow.h"
#include <QDebug>

ClientMainWindow::ClientMainWindow(QWidget *parent) :
	QMainWindow(parent),
	stopPB(tr("Отключить удаленное подключение"), this)
{
	connect(&stopPB, SIGNAL(clicked()), this, SLOT(close()));
	setCentralWidget(&stopPB);
}

void ClientMainWindow::info(quint8 type, const QString &data)
{
	qDebug()<<type<<data;
}

