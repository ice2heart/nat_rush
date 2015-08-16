#include "rawserver.h"
#include <QDataStream>

RawServer::RawServer(quint32 port, QObject *parent)
	:QObject(parent)
	,mPort(port)
{
	mServer = new QTcpServer(this);
	connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void RawServer::start()
{
	if (!mServer->listen(QHostAddress::Any,mPort))
	{
		qWarning()<<"Server start failure";
	}
	NR::Log(QString("Raw server start %1").arg(mPort), 0);
	emit serverStart(mPort);
}

RawServer::~RawServer()
{
	NR::Log("Destroy raw server", 3);
}

void RawServer::newConnection()
{
	NR::Log("New connection", 3);
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
	QByteArray temp = socket->readAll();
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
	NR::Log(QString("Raw server incoming data length %1").arg(data.length()), 6);
	if (s)
		s->write(data);
}
