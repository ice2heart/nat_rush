#include "rawclient.h"
#include <QDataStream>

RawClient::RawClient(QObject *parent) :
    QObject(parent)
{
    mSocket = new QTcpSocket(this);
    //mSocket->connectToHost("127.0.0.1", 5000);
    mSocket->connectToHost("ya.ru", 80);
    //mSocket->connectToHost("192.168.1.100", 6600); //home mdp
    connect(mSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void RawClient::connected()
{
}

void RawClient::readyRead()
{
    if(Q_UNLIKELY(mSocket->bytesAvailable() <= 0))
        return;
    QDataStream in(mSocket);
    int ba = mSocket->bytesAvailable();
    QByteArray tempBa;
    while (ba != 0)
    {
        if (ba > BUFSIZE)
        {
            in.readRawData(buf, BUFSIZE);
            tempBa.append(buf, BUFSIZE);
            ba -= BUFSIZE;
        }
        else
        {
            in.readRawData(buf, ba);
            tempBa.append(buf, ba);
            ba = 0;
        }
    }
    qDebug()<<QString::fromLocal8Bit(tempBa);
    emit newData(tempBa);
}

void RawClient::sendRawData(const QByteArray &data)
{
    qDebug()<<"send Raw data"<<data;
    mSocket->write(data);
}
