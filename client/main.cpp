#include <QCoreApplication>
#include "coreclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CoreClient cc;
    cc.test();

    return a.exec();
}
