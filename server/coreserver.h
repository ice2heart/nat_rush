#ifndef CORESERVER_H
#define CORESERVER_H

#include "../common/common.h"
#include <QTcpServer>
#include <QMap>
#include "rawserver.h"

class ConnectionStorage : public QObject
{
	Q_OBJECT
public:
	ConnectionStorage(intPool::spItem portShift, QTcpSocket *socket, QObject *parent = 0);
	~ConnectionStorage();
	quint64 mNextBlockSize;
	RawServer *mRawServer;
	QTcpSocket *mSocket;
public slots:
	void rawClientIn(quint8 id);
	void rawClientOut(quint8 id);
	void incomingData(quint8 clientId, const QByteArray &data);
	void rawServerStarted(quint16 port);
public:
	intPool::spItem mPortShift;
};

typedef QSharedPointer<ConnectionStorage> sConStore;

class CoreServer : public QObject
{
	Q_OBJECT
public:
	explicit CoreServer(QObject *parent = 0);
signals:
	void listConnection(const QVector<connData> &data);
public slots:
	void newConnection();
	void readyRead();
	void disconnected();
	void sendText(QTcpSocket *socket, quint8 num);
	void sendError(QTcpSocket *socket, const QString &reason);
	void genListConnection();
private:
	QTcpServer *mMainServer;
	QMap<QTcpSocket*, sConStore> mCoreClients;
	intPool mIntPool;
	quint16 mMainPort;
	quint8 mLogLvl;
};

#endif // CORESERVER_H
