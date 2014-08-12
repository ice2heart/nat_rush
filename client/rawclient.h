#ifndef RAWCLIENT_H
#define RAWCLIENT_H

#include "../common/common.h"

class RawClient : public QObject
{
	Q_OBJECT
public:
	explicit RawClient(const QString &host, quint16 port, quint8 id, QObject *parent = 0);
	virtual ~RawClient();
signals:
	void newData(quint8 id, const QByteArray &data);
public slots:
	void connected();
	void readyRead();
	void sendRawData(const QByteArray &data);
private:
	QTcpSocket *mSocket;
	char buf[BUFSIZE];
	quint8 mId;
};

#endif // RAWCLIENT_H
