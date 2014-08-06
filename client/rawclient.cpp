#include "rawclient.h"
#include <QDataStream>

RawClient::RawClient(quint8 id, QObject *parent) :
	QObject(parent),
	mId(id)
{
	mSocket = new QTcpSocket(this);
	//mSocket->connectToHost("127.0.0.1", 5000);
	//mSocket->connectToHost("ya.ru", 80);
	mSocket->connectToHost("127.0.0.1", 5900);
	//mSocket->connectToHost("192.168.1.100", 6600); //home mdp
	connect(mSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

RawClient::~RawClient()
{
	qDebug()<<"~RawClient";
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
	qDebug()<<"Raw client redy"<<tempBa.length();
	emit newData(mId, tempBa);
}

void RawClient::sendRawData(const QByteArray &data)
{
	qDebug()<<"send Raw data"<<data.length();
	mSocket->write(data);
}
