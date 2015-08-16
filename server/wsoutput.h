#ifndef WSOUTPUT_H
#define WSOUTPUT_H

#include "../common/common.h"
#include <QStringList>


QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WSOutput : public QObject
{
	Q_OBJECT
public:
	explicit WSOutput(int port = 80, QObject *parent = 0);
public slots:
	void listConnection(const QVector<connData> &data);
private slots:
	void sendToRawClient(QWebSocket *pSocket, const QString &str);
	void processPong(quint64 elapsedTime);

	void sendToClient(int id, const QString &str);
	void sendAll(const QString &str);

	void newConection();
	void readClient(QString frame);
	void endConection();
signals:
	void getList();
private:
	QWebSocketServer* server;
	QMap<QWebSocket*, int> clients;
	int mIncr;
//FIX ME
};

#endif // WSOUTPUT_H
