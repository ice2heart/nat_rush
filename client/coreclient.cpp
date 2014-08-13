#include "coreclient.h"
#include <QTimer>
#include "../common/common.h"
#include <QSettings>

CoreClient::CoreClient(QObject *parent)
	:QObject(parent)
	,mNextBlockSize(0)
{
	QSettings setting("config.ini", QSettings::IniFormat);
	QString address = setting.value("address", QString("178.62.189.199")).toString();
	quint16 port = setting.value("port", 6900).toUInt();
	mRawHost = setting.value("rawAddress", QString("127.0.0.1")).toString();
	mRawPort = setting.value("rawPort", 5900).toUInt();
	quint8 logLvl = setting.value("logLevel", 0).toUInt();
	NR::SetLogLvl(logLvl);
	NR::Log(QString("Connect to %1:%2").arg(address).arg(port));
	mMainSocket = new QTcpSocket(this);
	mMainSocket->connectToHost(address, port);
	connect(mMainSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mMainSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(mMainSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void CoreClient::connected()
{
	sendVersion();
	test();
}

void CoreClient::readyRead()
{
	QDataStream in(mMainSocket);
	in.setVersion(QDataStream::Qt_4_6);
	for(;;)
	{
		if (!mNextBlockSize)
		{
			if(mMainSocket->bytesAvailable() < sizeof(quint64))
				break;
			in >> mNextBlockSize;
		}

		if (mMainSocket->bytesAvailable() < mNextBlockSize)
			break;

		NR::Log(QString("Block size %1").arg(mNextBlockSize), 6);
		QString text;
		quint8 number;
		quint8 clientId;
		in >> number;
		QByteArray tempBa;
		switch (number) {
		case TEXTDATA:
			in >> text;
			NR::Log(QString("Ping data %1").arg(text), 6);
			QTimer::singleShot(50000, this, SLOT(test()));
			break;
		case CLIENTIN:
			in >> clientId;
			NR::Log(QString("Client in %1").arg(clientId), 3);
			clientIn(clientId);
			break;
		case CLIENTOUT:
			in >> clientId;
			NR::Log(QString("Client out %1").arg(clientId), 3);
			clientOut(clientId);
			break;
		case RAWDATA:
			in >> clientId;
			in >> tempBa;
			if (mRawClients.contains(clientId))
				mRawClients[clientId]->sendRawData(tempBa);
			NR::Log(QString("Incoming rawdata size %1").arg(tempBa.length()), 6);
		default:
			break;
		}
		mNextBlockSize = 0;
	}
}

void CoreClient::test()
{
	if (!mMainSocket)
		return;

	NR::writeToSocket(mMainSocket, TEXTDATA, 0);
}

void CoreClient::sendVersion()
{
	NR::writeToSocket(mMainSocket, PROTODATA, gCurrentVersion);
}


void CoreClient::incomingData(quint8 id, const QByteArray &data)
{
	if (!mMainSocket)
		return;

	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	out << quint8(id) << data;

	NR::Log(QString("Send raw bytes size %1").arg(dataBlock.size()), 6);
	NR::writeToSocket(mMainSocket, RAWDATA, dataBlock);
}

void CoreClient::clientIn(quint8 id)
{
	mRawClients.insert(id, new RawClient(mRawHost, mRawPort, id, this));
	connect(mRawClients[id], SIGNAL(newData(quint8,QByteArray)), this, SLOT(incomingData(quint8,QByteArray)));
}

void CoreClient::clientOut(quint8 id)
{
	RawClient *temp = mRawClients[id];
	delete temp;
	mRawClients.remove(id);
}

void CoreClient::disconnected()
{
	exit(0);
}
