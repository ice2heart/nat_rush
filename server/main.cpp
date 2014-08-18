#include <QCoreApplication>
#include "coreserver.h"
#include "wsoutput.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	CoreServer server;
	WSOutput output;
	return a.exec();
}
