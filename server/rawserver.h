#ifndef RAWSERVER_H
#define RAWSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>


class RawServer : public QObject
{
	Q_OBJECT
public:
	explicit RawServer(quint32 port, QObject *parent = 0);
	~RawServer();

signals:
	void newData(quint8 clientId, const QByteArray &data);
	void clientIn(quint8 clientId);
	void clientOut(quint8 clientId);
public slots:
	void newConnection();

	void readyRead();
	void disconnected();
	void incomingData(quint8 clientId, const QByteArray &data);
private:
	QTcpServer *mServer;
	QMap<QTcpSocket*, quint8> mSockets;
	char *buf;
	quint32 mPort;
};

#endif // RAWSERVER_H
