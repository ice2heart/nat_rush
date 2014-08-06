#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QDataStream>
#include <QTcpSocket>

const static quint32 BUFSIZE = 5000;
const static quint8 SERVERSTART = 2;
const static quint8 CLIENTIN = 3;
const static quint8 CLIENTOUT = 4;

namespace NR {

void writeToSocket(QTcpSocket *client, quint8 type, const QByteArray &qba);
void writeToSocket(QTcpSocket *client, quint8 type, quint8 data);

}
#endif // COMMON_H
