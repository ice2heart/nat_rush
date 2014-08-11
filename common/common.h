#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QDataStream>
#include <QTcpSocket>
#include <QSharedPointer>

const static quint32 BUFSIZE = 5000;
const static quint8 SERVERSTART = 2;
const static quint8 CLIENTIN = 3;
const static quint8 CLIENTOUT = 4;

namespace NR {

void writeToSocket(QTcpSocket *client, quint8 type, const QByteArray &qba);
void writeToSocket(QTcpSocket *client, quint8 type, quint8 data);

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
