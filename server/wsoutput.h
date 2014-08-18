#ifndef WSOUTPUT_H
#define WSOUTPUT_H

#include <QObject>
#include <QString>
#include "QWsServer.h"
#include "QWsSocket.h"
#include <QMap>

class WSOutput : public QObject
{
	Q_OBJECT
public:
	explicit WSOutput(int port = 80, QtWebsocket::Protocol protocol = QtWebsocket::Tcp, QObject *parent = 0);
public slots:
	void sendToRawClient(QtWebsocket::QWsSocket *pSocket, const QString &str);
	void processPong(quint64 elapsedTime);

	void sendToClient(int id, const QString &str);
	void sendAll(const QString &str);

private slots:
	void newConection();
	void readClient(QString frame);
	void endConection();

private:
	QtWebsocket::QWsServer* server;
	QMap<QtWebsocket::QWsSocket*, int> clients;
	int mIncr; //FIX ME};

#endif // WSOUTPUT_H
