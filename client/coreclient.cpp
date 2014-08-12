#include "coreclient.h"
#include <QTimer>
#include "../common/common.h"
#include <QSettings>

CoreClient::CoreClient(QObject *parent) :
	QObject(parent)
{
	QSettings setting("config.ini", QSettings::IniFormat);
	QString address = setting.value("address", QString("178.62.189.199")).toString();
	quint16 port = setting.value("port", 6900).toUInt();
	setting.sync();
	NR::Log(QString("Connect to %1:%2").arg(address).arg(port));
	mNextBlockSize = 0;
	mMainSocket = new QTcpSocket(this);
	mMainSocket->connectToHost(address, port);
	connect(mMainSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mMainSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(mMainSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void CoreClient::connected()
{

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

		qDebug()<<"Block size"<<mNextBlockSize;
		QString text;
		quint8 number;
		quint8 clientId;
		in >> number;
		QByteArray tempBa;
		switch (number) {
		case 1:
			in >> text;
			qDebug()<<text;
			QTimer::singleShot(50000, this, SLOT(test()));
			break;
		case CLIENTIN:
			in >> clientId;
			qDebug()<<"Client in"<<clientId;
			clientIn(clientId);
			break;
		case CLIENTOUT:
			in >> clientId;
			qDebug()<<"Client out"<<clientId;
			clientOut(clientId);
			break;
		case RAWDATA:
			in >> clientId;
			in >> tempBa;
			if (mRawClients.contains(clientId))
				mRawClients[clientId]->sendRawData(tempBa);
			qDebug()<<"rawdata"<<tempBa.length();
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

	NR::writeToSocket(mMainSocket, 1, 0);
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
	mRawClients.insert(id, new RawClient(id, this));
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
