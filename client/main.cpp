#include <QCoreApplication>
#include "coreclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CoreClient cc;

    return a.exec();
}
