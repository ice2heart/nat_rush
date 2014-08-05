#ifndef RAWSERVER_H
#define RAWSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <common.h>

class RawServer : public QObject
{
    Q_OBJECT
public:
    explicit RawServer(QObject *parent = 0);
    ~RawServer();

signals:
    void newData(const QByteArray &data);
public slots:
    void newConnection();

    void readyRead();
    void disconnected();
    void incomingData(const QByteArray &data);
private:
    QTcpServer *mServer;
    QList<QTcpSocket*> mSockets;
    char buf[BUFSIZE];
};

#endif // RAWSERVER_H
