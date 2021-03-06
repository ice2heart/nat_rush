#include "coreserver.h"
#include "QSettings"

CoreServer::CoreServer(QObject *parent)
	:QObject(parent)
	,mIntPool()
{
	QSettings setting("config.ini", QSettings::IniFormat);
	mMainPort = setting.value("MainPort", 6900).toUInt();
	setting.setValue("MainPort", mMainPort);
	mLogLvl = setting.value("LogLevel", 3).toUInt();
	setting.setValue("LogLevel", mLogLvl);
	setting.sync();

	mMainServer = new QTcpServer(this);
	for (quint16 i = 0; i<1000; i++)
		mIntPool.addItem(i);
	connect(mMainServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	if (!mMainServer->listen(QHostAddress::Any,mMainPort))
	{
		NR::Log("Server start failure");
		NR::Log(QString("FATAL ERROR: %1").arg(mMainServer->errorString()));
		exit(2);
	}
	NR::SetLogLvl(mLogLvl);
	NR::Log("Ready!");
}

void CoreServer::newConnection()
{
	QTcpSocket *newClient = mMainServer->nextPendingConnection();

	//Тут потенциально опасное место... если нехватает в пуле мест :(
	mCoreClients.insert(newClient, sConStore(new ConnectionStorage(mIntPool.acquire(), newClient)));
	connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(newClient, SIGNAL(readyRead()), this, SLOT(readyRead()));
	sConStore client = mCoreClients[newClient];
	client->mNextBlockSize = 0;
	genListConnection();
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
		quint8 version;
		QByteArray tempBa;
		NR::Log(QString("command %1").arg(command), 6);
		switch (command) {
		case TEXTDATA:
			in >> conNum;
			sendText(socket, ++ii);
			break;
		case PROTODATA:
			in >> version;
			if (version != gCurrentVersion)
			{
				sendError(socket, "Bad version");
				socket->close();
			}
			break;
		case RAWDATA:
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
	NR::Log("Client gone");
	QTcpSocket *socket = (QTcpSocket *) sender();
	mIntPool.release(mCoreClients[socket]->mPortShift);
	mCoreClients.remove(socket);
	genListConnection();
}

void CoreServer::sendText(QTcpSocket *socket, quint8 num)
{
	NR::writeToSocket<quint8>(socket, TEXTDATA, sizeof(quint8), num);
}

void CoreServer::sendError(QTcpSocket *socket, const QString &reason)
{
	NR::Log(QString("ERROR: Send error msg reason: %1").arg(reason));
	NR::writeToSocket<QString>(socket, ERRORMSG, sizeof(reason), reason);
}

void CoreServer::genListConnection()
{
	QVector<connData> vect;

	foreach (sConStore conStore, mCoreClients) {
		connData conn;
		conn.ip = conStore->mSocket->peerAddress().toString();
		conn.port = *(conStore->mPortShift) + BASEPORT;
		conn.id = *(conStore->mPortShift);
		vect.append(conn);
	}
	emit listConnection(vect);
}

ConnectionStorage::ConnectionStorage(intPool::spItem portShift, QTcpSocket *socket, QObject *parent)
	:QObject(parent)
	,mPortShift(portShift)
	,mSocket(socket)
{
	mRawServer = new RawServer((*mPortShift)+BASEPORT, this);
	connect(mRawServer, SIGNAL(serverStart(quint16)), this, SLOT(rawServerStarted(quint16)));
	connect(mRawServer, SIGNAL(newData(quint8,QByteArray)), this, SLOT(incomingData(quint8,QByteArray)));
	connect(mRawServer, SIGNAL(clientIn(quint8)), this, SLOT(rawClientIn(quint8)));
	connect(mRawServer, SIGNAL(clientOut(quint8)), this, SLOT(rawClientOut(quint8)));
	mRawServer->start();
}

ConnectionStorage::~ConnectionStorage()
{
	delete mRawServer;
}
void ConnectionStorage::rawClientIn(quint8 id)
{
	NR::Log(QString("Client in %1").arg(id), 3);
	NR::writeToSocket<quint8>(mSocket,CLIENTIN, sizeof(quint8), id);
}

void ConnectionStorage::rawClientOut(quint8 id)
{
	NR::Log(QString("Client out %1").arg(id), 3);
	NR::writeToSocket<quint8>(mSocket,CLIENTOUT, sizeof(quint8), id);
}

void ConnectionStorage::incomingData(quint8 clientId, const QByteArray &data)
{
	NR::writeToSocket<quint8, QByteArray>(mSocket, RAWDATA, sizeof(quint8) + data.size(), clientId, data);
}

void ConnectionStorage::rawServerStarted(quint16 port)
{
	NR::Log(QString("Send info about port %1").arg(port), 3);
	NR::writeToSocket<quint16>(mSocket, RAWSERVERSTART, sizeof(quint16), quint16(port));
}
