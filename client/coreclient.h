#ifndef CORECLIENT_H
#define CORECLIENT_H

#include "../common/common.h"
#include <QMap>
#include "rawclient.h"
#include "QProcess"

class CoreClient : public QObject
{
	Q_OBJECT
public:
	explicit CoreClient(QObject *parent = 0);
	virtual ~CoreClient();

signals:
	void connectionInfo(quint8 type, const QString &text);
public slots:
	void tryConnect();
	void connected();
	void readyRead();
	void test();
	void sendVersion();
	void incomingData(quint8 id, const QByteArray &data);
	void clientIn(quint8 id);
	void clientOut(quint8 id);
	void disconnected();
	void error(QAbstractSocket::SocketError socketState);
	void endProcess(int exitCode);
private:
	QTcpSocket *mMainSocket;
	quint64 mNextBlockSize;
	QString mHostName;
	quint16 mHostPort;

	QString mRawHost;
	quint16 mRawPort;
	QMap<quint8, RawClient*> mRawClients;
	QString mProcessName;
	QString mProcessParam;
	QProcess vncProcess;
};

#endif // CORECLIENT_H
