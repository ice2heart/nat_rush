#include "common.h"
#include <QDebug>

static quint8 gLogLvl = 0;

namespace NR {

void writeToSocket(QTcpSocket *client, quint8 type, const QByteArray &qba)
{
	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_4_6);
	out << quint64(qba.size() + sizeof(quint8)) << quint8(type) ;

	client->write(data);
	client->write(qba);
}
void writeToSocket(QTcpSocket *client, quint8 type, quint8 data)
{
	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	out << quint8(data);
	writeToSocket(client, type, dataBlock);
}

void writeToSocket(QTcpSocket *client, quint8 type, quint16 data)
{
	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	out << quint16(data);
	writeToSocket(client, type, dataBlock);
}

void writeToSocket(QTcpSocket *client, quint8 type, const QString &str)
{
	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out << str;
	writeToSocket(client, type, dataBlock);
}

void SetLogLvl(quint8 logLvl)
{
	gLogLvl = logLvl;
}

void Log(const QString &msg, quint8 logLvl)
{
	if (logLvl <= gLogLvl)
		qDebug()<<msg;
}
}

