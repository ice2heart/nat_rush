#include <QCoreApplication>
#include "coreserver.h"
#include "wsoutput.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	CoreServer server;
	WSOutput output(8080);
	QObject::connect(&server, &CoreServer::listConnection, &output, &WSOutput::listConnection);
	QObject::connect(&output, &WSOutput::getList, &server, &CoreServer::genListConnection);
	return a.exec();
}
