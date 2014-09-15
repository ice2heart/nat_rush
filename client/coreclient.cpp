#include "coreclient.h"
#include <QTimer>
#include <QStringList>
#include <QSettings>
#include <QApplication>

CoreClient::CoreClient(QObject *parent)
	:QObject(parent)
	,mNextBlockSize(0)
{
#ifdef Q_OS_LINUX
	QSettings setting(qApp->applicationDirPath() + "/config.ini", QSettings::IniFormat);
#else
	QSettings setting("config.ini", QSettings::IniFormat);
#endif
	mHostName = setting.value("address", QString("178.62.189.199")).toString();
	mHostPort = setting.value("port", 6900).toUInt();
	mRawHost = setting.value("rawAddress", QString("127.0.0.1")).toString();
	mRawPort = setting.value("rawPort", 5900).toUInt();
	quint8 logLvl = setting.value("logLevel", 0).toUInt();
	mProcessName = setting.value("process","winvnc.exe").toString();
	mProcessParam = setting.value("processArgs", "").toString();
	if (mProcessName.compare("none"))
	{
		vncProcess.start(mProcessName, mProcessParam.split(' '));
		vncProcess.waitForStarted();
		connect(&vncProcess, SIGNAL(finished(int)), this, SLOT(endProcess(int)));
		connect(this, SIGNAL(destroyed()), &vncProcess, SLOT(terminate()));
	}
	NR::SetLogLvl(logLvl);
	mMainSocket = new QTcpSocket(this);
	connect(mMainSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mMainSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(mMainSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(mMainSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
	tryConnect();
}

void CoreClient::tryConnect()
{
	NR::Log(QString("Connect to %1:%2").arg(mHostName).arg(mHostPort));
	emit connectionInfo(CLIENT_SERVER_OK, tr("Подключение..."));
	mMainSocket->connectToHost(mHostName, mHostPort);
}

CoreClient::~CoreClient()
{
	vncProcess.close();
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
		quint16 serverPort;
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
		case RAWSERVERSTART:
			in >> serverPort;
			emit connectionInfo(CLIENT_SERVER_OK, tr("Вы клиент номер %1").arg(serverPort));
			NR::Log(QString("Server start in port %1").arg(serverPort), 3);
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

	NR::writeToSocket(mMainSocket, TEXTDATA, quint8(0));
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
	//qApp->exit();
	mMainSocket->abort();
	mMainSocket->close();
}

void CoreClient::error(QAbstractSocket::SocketError socketState)
{
	switch (socketState)
	{
	case QAbstractSocket::AddressInUseError:
		NR::Log("SOCKET ERROR: Address is already in use");
		emit connectionInfo(CLIENT_SERVER_ERROR, tr("Ошибка повторного использования аддреса.\nПерподключение через 5 сек."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		NR::Log("SOCKET ERROR: Connection refused");
		emit connectionInfo(CLIENT_SERVER_ERROR, tr("Сервер неотвечает, проблема с подключением.\nПерподключение через 5 сек."));
		break;
	case QAbstractSocket::HostNotFoundError:
		NR::Log("SOCKET ERROR: Host not found");
		emit connectionInfo(CLIENT_SERVER_ERROR, tr("Сетевая ошибка, не найден сервер.\nПерподключение через 5 сек."));
		break;
	case QAbstractSocket::RemoteHostClosedError:
		NR::Log("SOCKET ERROR: Remote host closed");
		emit connectionInfo(CLIENT_SERVER_ERROR, tr("Сервер разорвал соединение.\nПерподключение через 5 сек."));
		break;
	default:
		NR::Log("SOCKET ERROR:unknown error");
		emit connectionInfo(CLIENT_SERVER_ERROR, tr("Ошибка соединения.\nПерподключение через 5 сек."));
		break;
	}
	mMainSocket->abort();
	mMainSocket->close();
	QTimer::singleShot(5000, this, SLOT(tryConnect()));
}

void CoreClient::endProcess(int exitCode)
{
	if (exitCode == 0)
		return;
	NR::Log(QString("Exit code %1").arg(exitCode));
	NR::Log(QString(vncProcess.readAllStandardError()), 0);
}
