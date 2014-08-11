#include "common.h"

namespace NR {

void writeToSocket(QTcpSocket *client, quint8 type, const QByteArray &qba)
{
	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_4_6);
	out << quint64(qba.size() + sizeof(quint8)) << quint8(type) ;

	client->write(data);
	client->write(qba);
}
void writeToSocket(QTcpSocket *client, quint8 type, quint8 data)
{
	QByteArray dataBlock;
	QDataStream out(&dataBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_6);
	out << quint8(data);
	writeToSocket(client, type, dataBlock);
}
/*
template <class T> void Pool<T>::addItem(T item)
{
	mPool.insert(QSharedPointer<T>(new T(item)), -1);
}

template <class T> typename Pool<T>::spItem Pool<T>::acquire()
{
	spItem item = mPool.key(-1);
	mPool.insert(item, 1);
	return item;
}

template <class T> void Pool<T>::release(spItem item)
{
	mPool.insert(item, -1);
}*/
}

