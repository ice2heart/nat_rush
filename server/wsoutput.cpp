#include "wsoutput.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"

WSOutput::WSOutput(int port, QObject *parent)
	:QObject(parent)
	, server(new QWebSocketServer(QStringLiteral("Echo Server"),
								QWebSocketServer::NonSecureMode, this))
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
	connect(server, &QWebSocketServer::newConnection, this, &WSOutput::newConection);
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

void WSOutput::sendToRawClient(QWebSocket *pSocket, const QString &str)
{
	pSocket->sendTextMessage(str);
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
	foreach(QWebSocket *pClientSocket, clients.keys())
	{
		sendToRawClient(pClientSocket, str);
	}
}

void WSOutput::newConection()
{
	QWebSocket* clientSocket = server->nextPendingConnection();
	connect(clientSocket, &QWebSocket::disconnected, this, &WSOutput::endConection);
	connect(clientSocket, &QWebSocket::pong, this, &WSOutput::processPong);
	//Бладж не перегружайте слоты!
	connect(clientSocket, &QWebSocket::textMessageReceived,
			this, &WSOutput::readClient);
	clients.insert(clientSocket, ++mIncr);
	emit getList();
}

void WSOutput::readClient(QString frame)
{
	QWebSocket* socket = qobject_cast<QWebSocket*>(sender());
	emit getList();
	//sendToRawClient(socket, tr("ok"));

}

void WSOutput::endConection()
{
	QWebSocket* socket = qobject_cast<QWebSocket*>(sender());

	//int key = clients[socket];
	socket->deleteLater();
	clients.remove(socket);

}
