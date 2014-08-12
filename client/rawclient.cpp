#include "rawclient.h"

RawClient::RawClient(const QString &host, quint16 port, quint8 id, QObject *parent) :
	QObject(parent),
	mId(id)
{
	mSocket = new QTcpSocket(this);
	connect(mSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	mSocket->connectToHost(host, port);
}

RawClient::~RawClient()
{
	NR::Log("~RawClient", 6);
}

void RawClient::connected()
{
}

void RawClient::readyRead()
{
	if(Q_UNLIKELY(mSocket->bytesAvailable() <= 0))
		return;
	QDataStream in(mSocket);
	int ba = mSocket->bytesAvailable();
	QByteArray tempBa;
	while (ba != 0)
	{
		if (ba > BUFSIZE)
		{
			in.readRawData(buf, BUFSIZE);
			tempBa.append(buf, BUFSIZE);
			ba -= BUFSIZE;
		}
		else
		{
			in.readRawData(buf, ba);
			tempBa.append(buf, ba);
			ba = 0;
		}
	}
	emit newData(mId, tempBa);
}

void RawClient::sendRawData(const QByteArray &data)
{
	NR::Log(QString("send Raw data size %1").arg(data.length()), 6);
	mSocket->write(data);
}
