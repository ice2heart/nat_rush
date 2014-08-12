#include "coreserver.h"

CoreServer::CoreServer(QObject *parent)
	:QObject(parent)
	,mIntPool()
{
	mMainServer = new QTcpServer(this);
	for (quint8 i = 0; i<50; i++)
		mIntPool.addItem(i);
	connect(mMainServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	if (!mMainServer->listen(QHostAddress::Any,6900))
	{
		qWarning()<<"Server start failure";
	}
	NR::Log("Ready!");
}

void CoreServer::newConnection()
{
	QTcpSocket *newClient = mMainServer->nextPendingConnection();

	mCoreClients.insert(newClient, sConStore(new ConnectionStorage(mIntPool.acquire())));
	connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(newClient, SIGNAL(readyRead()), this, SLOT(readyRead()));
	sConStore client = mCoreClients[newClient];
	client->mNextBlockSize = 0;
	client->mSocket = newClient;
	connect(client->mRawServer, SIGNAL(newData(quint8,QByteArray)), this, SLOT(incomingRawData(quint8,QByteArray)));
	connect(client->mRawServer, SIGNAL(clientIn(quint8)), client.data(), SLOT(rawClientIn(quint8)));
	connect(client->mRawServer, SIGNAL(clientOut(quint8)), client.data(), SLOT(rawClientOut(quint8)));
}

void CoreServer::readyRead()
{
	QTcpSocket *socket = (QTcpSocket *) sender();
	sConStore tmpCon = mCoreClients[socket];

	NR::Log(QString("Main server read data %1").arg(tmpCon->mNextBlockSize), 6);
	QDataStream in(socket);
	in.setVersion(QDataStream::Qt_4_6);
	for(;;)
	{
		if (!tmpCon->mNextBlockSize)
		{
			if(socket->bytesAvailable() < sizeof(quint64))
				break;
			in >> tmpCon->mNextBlockSize;

		}

		if (socket->bytesAvailable() < tmpCon->mNextBlockSize)
			break;

		quint8 command;
		in >> command;
		static int ii = 0;
		quint8 conNum;
		QByteArray tempBa;
		NR::Log(QString("command %1").arg(command), 3);
		switch (command) {
		case 1:
			sendText(socket, QString::number(++ii));
			break;
		case 99:
			in >> conNum;
			in >> tempBa;
			NR::Log(QString("New raw data length %1").arg(tempBa.length()), 6);
			tmpCon->mRawServer->incomingData(conNum, tempBa);
			break;
		default:
			break;
		}
		tmpCon->mNextBlockSize = 0;
	}
}

void CoreServer::disconnected()
{
	qDebug()<<"Client gone";
	QTcpSocket *socket = (QTcpSocket *) sender();
	mCoreClients.remove(socket);
}

void CoreServer::sendText(QTcpSocket *socket, const QString &text)
{
	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);

	out << quint64(0) << quint8(1) << text;
	out.device()->seek(0);
	out << quint64(dataBlock.size() - sizeof(quint64));

	socket->write(dataBlock);
}

void CoreServer::incomingRawData(quint8 id, const QByteArray &data)
{
	foreach (QTcpSocket *socket, mCoreClients.keys()) {
		QByteArray dataBlock;
		QDataStream out(&dataBlock, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_6);

		out << quint64(0) << quint8(99)<<quint8(id); //0 заменить на что то полезное
		out << data;
		out.device()->seek(0);
		out << quint64(dataBlock.size() - sizeof(quint64));

		socket->write(dataBlock);
	}
}

ConnectionStorage::ConnectionStorage(intPool::spItem portShift, QObject *parent)
	:QObject(parent)
	,mPortShift(portShift)
{

	mRawServer = new RawServer((*mPortShift)+8000, this);
}

ConnectionStorage::~ConnectionStorage()
{
	delete mRawServer;
}
void ConnectionStorage::rawClientIn(quint8 id)
{
	NR::Log(QString("Client in").arg(id), 3);
	NR::writeToSocket(mSocket,CLIENTIN, id);
}

void ConnectionStorage::rawClientOut(quint8 id)
{
	NR::Log(QString("Client out").arg(id), 3);
	NR::writeToSocket(mSocket,CLIENTOUT, id);
}
