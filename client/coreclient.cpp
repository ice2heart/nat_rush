#include "coreclient.h"
#include <QDebug>
#include <QTimer>
#include "../common/common.h"

CoreClient::CoreClient(QObject *parent) :
	QObject(parent)
{
	//mNextblock Надо вынести в мапку
	mNextBlockSize = 0;
	mMainSocket = new QTcpSocket(this);
	mMainSocket->connectToHost("178.62.189.199", 6900);
	connect(mMainSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mMainSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void CoreClient::connected()
{
	/*mRawClient = new RawClient(this);
	*/

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
		case 99:
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

	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	out << quint64(0) << quint8(1); //screen request

	out.device()->seek(0);
	out << quint64(dataBlock.size() - sizeof(quint64));

	qDebug()<<"client: send byte" << dataBlock.size();
	mMainSocket->write(dataBlock);
}


void CoreClient::incomingData(quint8 id, const QByteArray &data)
{
	if (!mMainSocket)
		return;

	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	out << quint64(0) << quint8(99)<<quint8(id); //поправить формат сереализации
	out << data;
	out.device()->seek(0);
	out << quint64(dataBlock.size() - sizeof(quint64));

	qDebug()<<"client: send byte" << dataBlock.size();
	mMainSocket->write(dataBlock);
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
