#include "wsoutput.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

WSOutput::WSOutput(int port, QtWebsocket::Protocol protocol, QObject *parent)
	:QObject(parent)
	, server(new QtWebsocket::QWsServer(this, protocol))
	,mIncr(0)
{
	if (! server->listen(QHostAddress::Any, port))
	{
		qDebug() << tr("Error: Can't launch server");
		qDebug() << tr("QWsServer error : %1").arg(server->errorString());
	}
	else
	{
		qDebug() << tr("Server is listening on port %1").arg(port);
	}
	connect(server, &QtWebsocket::QWsServer::newConnection, this, &WSOutput::newConection);
}

void WSOutput::listConnection(const QVector<connData> &data)
{
	QJsonObject mainObj;
	QJsonArray mainArr;
	foreach (connData conn, data) {
		QJsonObject connObj;
		connObj.insert("ip", conn.ip);
		connObj.insert("port", conn.port);
		connObj.insert("id", conn.id);
		mainArr.append(connObj);
	}
	mainObj.insert("data",mainArr);
	QJsonDocument doc(mainObj);
	QString output(doc.toJson());
	NR::Log(output, 3);
	sendAll(output);
}

void WSOutput::sendToRawClient(QtWebsocket::QWsSocket *pSocket, const QString &str)
{
	pSocket->write(str);
}

void WSOutput::processPong(quint64 elapsedTime)
{
	qDebug() << tr("ping: %1 ms").arg(elapsedTime);
}

void WSOutput::sendToClient(int id, const QString &str)
{
	sendToRawClient(clients.key(id), str);
}

void WSOutput::sendAll(const QString &str)
{
	foreach(QtWebsocket::QWsSocket *pClientSocket, clients.keys())
	{
		sendToRawClient(pClientSocket, str);
	}
}

void WSOutput::newConection()
{
	QtWebsocket::QWsSocket* clientSocket = server->nextPendingConnection();
	connect(clientSocket, &QtWebsocket::QWsSocket::disconnected, this, &WSOutput::endConection);
	connect(clientSocket, &QtWebsocket::QWsSocket::pong, this, &WSOutput::processPong);
	//Бладж не перегружайте слоты!
	connect(clientSocket, static_cast<void (QtWebsocket::QWsSocket::*)(QString)>(&QtWebsocket::QWsSocket::frameReceived),
			this, &WSOutput::readClient);
	clients.insert(clientSocket, ++mIncr);
}

void WSOutput::readClient(QString frame)
{
	QtWebsocket::QWsSocket* socket = qobject_cast<QtWebsocket::QWsSocket*>(sender());
	emit getList();
	sendToRawClient(socket, tr("ok"));

}

void WSOutput::endConection()
{
	QtWebsocket::QWsSocket* socket = qobject_cast<QtWebsocket::QWsSocket*>(sender());

	int key = clients[socket];
	socket->deleteLater();
	clients.remove(socket);

}
