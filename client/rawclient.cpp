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
	qDebug()<<__FUNCTION__<<mSocket->bytesAvailable();
	QDataStream in(mSocket);
	int ba = mSocket->bytesAvailable();
	QByteArray tempBa = mSocket->readAll();
	emit newData(mId, tempBa);
}

void RawClient::sendRawData(const QByteArray &data)
{
	NR::Log(QString("send Raw data size %1").arg(data.length()), 6);
	mSocket->write(data);
}
