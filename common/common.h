#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QDataStream>
#include <QTcpSocket>
#include <QSharedPointer>
#include <QMap>

const static quint32 BUFSIZE = 10000;
const static quint16 BASEPORT = 7000;
const static quint8 SERVERSTART = 2;
const static quint8 CLIENTIN = 3;
const static quint8 CLIENTOUT = 4;
const static quint8 TEXTDATA = 1;
const static quint8 RAWDATA = 99;
const static quint8 PROTODATA = 5;
const static quint8 ERRORMSG = 6;
const static quint8 RAWSERVERSTART = 7;

const static quint8 CLIENT_SERVER_OK = 1;
const static quint8 CLIENT_SERVER_ERROR = 2;

const static quint8 gCurrentVersion = 3;
struct connData
{
	quint8 id;
	QString ip;
	quint16 port;
};

namespace NR {

template<typename T> void writeToSocket(QTcpSocket *client, quint8 type, quint32 size, const T &data)
{
	QByteArray qba;
	QDataStream out(&qba, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_5_4);
	out << quint64(size + sizeof(quint8)) << quint8(type) << data;

	client->write(qba);
}

template<typename T1, typename T2> void writeToSocket(QTcpSocket *client, quint8 type, quint32 size, const T1 &data, const T2 &data2)
{
	QByteArray qba;
	QDataStream out(&qba, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_5_4);
	out << quint64(size + sizeof(quint8)) << quint8(type) << data << data2;

	client->write(qba);
}

void Log(const QString &msg, quint8 logLvl = 0);
void SetLogLvl(quint8 logLvl);

template <class T> class Pool
{
public:
	typedef QSharedPointer<T> spItem;
	Pool(){}
	virtual ~Pool(){}
	void addItem(T item){mPool.insert(QSharedPointer<T>(new T(item)), -1);}
	spItem acquire()
	{
		spItem item = mPool.key(-1);
		mPool.insert(item, 1);
		return item;
	}
	void release(spItem item){mPool.insert(item, -1);}
private:
	QMap<spItem, qint8> mPool;
};
}

typedef NR::Pool<quint16> intPool;
#endif // COMMON_H
