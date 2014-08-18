#include "wsoutput.h"

WSOutput::WSOutput(int port, QtWebsocket::Protocol protocol, QObject *parent)
{
}

void WSOutput::sendToRawClient(QtWebsocket::QWsSocket *pSocket, const QString &str)
{
}

void WSOutput::processPong(quint64 elapsedTime)
{
}

void WSOutput::sendToClient(int id, const QString &str)
{
}

void WSOutput::sendAll(const QString &str)
{
}

void WSOutput::newConection()
{
}

void WSOutput::readClient(QString frame)
{
}

void WSOutput::endConection()
{
}
