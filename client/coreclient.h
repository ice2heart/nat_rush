#ifndef CORECLIENT_H
#define CORECLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QMap>
#include "rawclient.h"

class CoreClient : public QObject
{
	Q_OBJECT
public:
	explicit CoreClient(QObject *parent = 0);

signals:

public slots:
	void connected();
	void readyRead();
	void test();
	void incomingData(quint8 id, const QByteArray &data);
	void clientIn(quint8 id);
	void clientOut(quint8 id);
private:
	QTcpSocket *mMainSocket;
	quint64 mNextBlockSize;

	QMap<quint8, RawClient*> mRawClients;
};

#endif // CORECLIENT_H
