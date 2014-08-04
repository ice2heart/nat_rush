#include "coreserver.h"
#include <QDebug>

CoreServer::CoreServer(QObject *parent) :
    QObject(parent)
{
    mMainServer = new QTcpServer(this);
    if (!mMainServer->listen(QHostAddress::Any,6900))
    {
        qWarning()<<"Server start failure";
    }
    connect(mMainServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    //сделать мультиюзер
    mRawServer = new RawServer(this);
    qDebug()<<"Ready!";
}

void CoreServer::newConnection()
{
    QTcpSocket *newClient = mMainServer->nextPendingConnection();
    mCoreClients.insert(newClient, 0);
    connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(newClient, SIGNAL(readyRead()), this, SLOT(readyRead()));

    connect(mRawServer, SIGNAL(newData(int,char*)), this, SLOT(incomingRawData(int,char*)));
}

void CoreServer::readyRead()
{
    QTcpSocket *socket = (QTcpSocket *) sender();
    quint64 mNextBlockSize = mCoreClients[socket];

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_6);
    for(;;)
    {
        if (!mNextBlockSize)
        {
            if(socket->bytesAvailable() < sizeof(quint64))
                break;
            in >> mNextBlockSize;

        }

        if (socket->bytesAvailable() < mNextBlockSize)
            break;

        quint8 command;
        in >> command;
        static int ii = 0;
        quint64 bSize;
        //тут надо поправить
        char bbuf[2000];
        switch (command) {
        case 1:
            qDebug()<<"Command"<<command;
            sendText(socket, QString::number(++ii));

            //sendRawData(socket, sizeof(a), a);
            break;
        case 99:
            in >> bSize;
            in.readRawData(bbuf, bSize);
            mRawServer->incomingData(bSize, bbuf);
        default:
            break;
        }
        mNextBlockSize = 0;
    }
    mCoreClients[socket] = mNextBlockSize;
}

void CoreServer::disconnected()
{
    qDebug()<<"Client gone";
    QTcpSocket *socket = (QTcpSocket *) sender();
    mCoreClients.remove(socket);
}

void CoreServer::sendText(QTcpSocket *socket, const QString &text)
{
    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << quint64(0) << quint8(1) << text;
    out.device()->seek(0);
    out << quint64(dataBlock.size() - sizeof(quint64));

    socket->write(dataBlock);
}

void CoreServer::sendRawData(QTcpSocket *socket, int bufsize, char *buf)
{
    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    out << quint64(0) << quint8(99)<<quint64(bufsize);
    out.writeRawData(buf, bufsize);

    out.device()->seek(0);
    out << quint64(dataBlock.size() - sizeof(quint64));

    socket->write(dataBlock);
}

void CoreServer::incomingRawData(int size, char *data)
{
    foreach (QTcpSocket *socket, mCoreClients.keys()) {
        sendRawData(socket, size, data);
    }
}
