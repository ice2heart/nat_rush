#include "rawserver.h"
#include <QDataStream>
#include <QDebug>

RawServer::RawServer(QObject *parent) :
    QObject(parent)
{
    mServer = new QTcpServer(this);
    if (!mServer->listen(QHostAddress::Any,8000))
    {
        qWarning()<<"Server start failure";
    }
    qDebug()<<"Raw server start "<< 8000;
    connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

RawServer::~RawServer()
{
    qDebug()<<"Destroy raw server";
}

void RawServer::newConnection()
{
    qDebug()<<"New connection";
    QTcpSocket *newClient = mServer->nextPendingConnection();
    mSockets.append(newClient);
    connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(newClient, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void RawServer::readyRead()
{
    QTcpSocket *socket = (QTcpSocket *) sender();
    if (socket->bytesAvailable() <= 0)
        return;
    QDataStream in(socket);
    quint64 ba = socket->bytesAvailable();
    QByteArray temp;
    while (ba != 0)
    {
        if (ba > BUFSIZE)
        {
            in.readRawData(buf, BUFSIZE);
            ba -= BUFSIZE;
            temp.append(buf, BUFSIZE);
        }
        else
        {
            in.readRawData(buf, ba);
            temp.append(buf, ba);
            ba = 0;
        }
    }
    emit newData(temp);
}

void RawServer::disconnected()
{
    QTcpSocket *socket = (QTcpSocket *) sender();
    mSockets.removeOne(socket);
    //надо бы сделать нормальный дисконект наверное disconnect()
}

/*void RawServer::incomingData(int size, char *data)
{
    qDebug()<<"raw server incoming data";
    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.writeRawData(data, size);
    foreach (QTcpSocket *s, mSockets) {
        qDebug()<<QString::fromLocal8Bit(data, size);
        s->write(dataBlock);
    }
}
*/
void RawServer::incomingData(const QByteArray &data)
{
    foreach (QTcpSocket *s, mSockets) {
        qDebug()<<QString::fromLocal8Bit(data);
        s->write(data);
    }
}
