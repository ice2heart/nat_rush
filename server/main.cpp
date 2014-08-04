#include <QCoreApplication>
#include "coreserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CoreServer server;
    return a.exec();
}
