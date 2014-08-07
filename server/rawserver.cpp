#include "rawserver.h"
#include <QDataStream>
#include <QDebug>
#include "../common/common.h"

RawServer::RawServer(QObject *parent) :
	QObject(parent)
{
	buf = new char[BUFSIZE]();
	mServer = new QTcpServer(this);
	if (!mServer->listen(QHostAddress::Any,8000))
	{
		qWarning()<<"Server start failure";
	}
	qDebug()<<"Raw server start "<< 8000;
	connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

RawServer::~RawServer()
{
	qDebug()<<"Destroy raw server";
}

void RawServer::newConnection()
{
	qDebug()<<"New connection";
	QTcpSocket *newClient = mServer->nextPendingConnection();
	static int counter = 0;
	mSockets.insert(newClient, ++counter);
	connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(newClient, SIGNAL(readyRead()), this, SLOT(readyRead()));
	emit clientIn(counter);
}

void RawServer::readyRead()
{
	QTcpSocket *socket = (QTcpSocket *) sender();
	if (!socket)
	{
		qWarning()<<"RawServer write to empty socket";
		return;
	}
	if (socket->bytesAvailable() <= 0)
		return;
	QDataStream in(socket);
	quint64 ba = socket->bytesAvailable();
	QByteArray temp;
	while (ba != 0)
	{
		if (ba > BUFSIZE)
		{
			in.readRawData(buf, BUFSIZE);
			ba -= BUFSIZE;
			temp.append(buf, BUFSIZE);
		}
		else
		{
			in.readRawData(buf, ba);
			temp.append(buf, ba);
			ba = 0;
		}
	}
	emit newData(mSockets[socket], temp);
}

void RawServer::disconnected()
{
	QTcpSocket *socket = (QTcpSocket *) sender();
	emit clientOut(mSockets[socket]);
	mSockets.remove(socket);
	//надо бы сделать нормальный дисконект наверное disconnect()
}

void RawServer::incomingData(quint8 clientId, const QByteArray &data)
{
	QTcpSocket *s = mSockets.key(clientId);
	qDebug()<<"Raw server Incoming data"<<data.length();
	if (s->isOpen())
		s->write(data);
}
