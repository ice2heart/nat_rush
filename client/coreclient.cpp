#include "coreclient.h"
#include <QDebug>
#include <QTimer>

CoreClient::CoreClient(QObject *parent) :
    QObject(parent)
{
    //mNextblock Надо вынести в мапку
    mNextBlockSize = 0;
    mMainSocket = new QTcpSocket(this);
    mMainSocket->connectToHost("127.0.0.1", 6900);
    connect(mMainSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(mMainSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void CoreClient::connected()
{
    mRawClient = new RawClient(this);
    connect(mRawClient, SIGNAL(newData(char*,int)), this, SLOT(incomingData(char*,int)));

}

void CoreClient::readyRead()
{
    QDataStream in(mMainSocket);
    in.setVersion(QDataStream::Qt_4_6);
    for(;;)
    {
        if (!mNextBlockSize)
        {
            if(mMainSocket->bytesAvailable() < sizeof(quint64))
                break;
            in >> mNextBlockSize;
        }

        if (mMainSocket->bytesAvailable() < mNextBlockSize)
            break;

        QString text;
        quint8 number;
        in >> number;
        char ss[2000];
        quint64 buffsize;
        if (number == 1)
        {
            in >> text;
            qDebug()<<text;
            QTimer::singleShot(50000, this, SLOT(test()));
        }
        else
        {
            in >> buffsize;
            in.readRawData(ss, buffsize);
            mRawClient->sendRawData(ss, buffsize);
            qDebug()<<"rawdata"<<QString::fromLocal8Bit(ss);
            //
        }
        mNextBlockSize = 0;
    }
}

void CoreClient::test()
{
    if (!mMainSocket)
        return;

    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint64(0) << quint8(1); //screen request

    out.device()->seek(0);
    out << quint64(dataBlock.size() - sizeof(quint64));

    qDebug()<<"pClient: send byte" << dataBlock.size();
    mMainSocket->write(dataBlock);
}

void CoreClient::incomingData(char *data, int len)
{
    if (!mMainSocket)
        return;

    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint64(0) << quint8(99)<<quint64(len);
    out.writeRawData(data, len);
    out.device()->seek(0);
    out << quint64(dataBlock.size() - sizeof(quint64));

    qDebug()<<"pClient: send byte" << dataBlock.size();
    mMainSocket->write(dataBlock);
}
