#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QDataStream>
#include <QTcpSocket>
#include <QSharedPointer>
#include <QMap>

const static quint32 BUFSIZE = 5000;
const static quint8 SERVERSTART = 2;
const static quint8 CLIENTIN = 3;
const static quint8 CLIENTOUT = 4;
const static quint8 TEXTDATA = 1;
const static quint8 RAWDATA = 99;
const static quint8 PROTODATA = 5;
const static quint8 ERRORMSG = 6;

const static quint8 gCurrentVersion = 1;

namespace NR {

void writeToSocket(QTcpSocket *client, quint8 type, const QByteArray &qba);
void writeToSocket(QTcpSocket *client, quint8 type, quint8 data);
void writeToSocket(QTcpSocket *client, quint8 type, const QString &str);

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

typedef NR::Pool<quint8> intPool;
#endif // COMMON_H
