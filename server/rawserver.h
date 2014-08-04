#ifndef RAWSERVER_H
#define RAWSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class RawServer : public QObject
{
    Q_OBJECT
public:
    explicit RawServer(QObject *parent = 0);
    ~RawServer();

signals:
    void newData(int size, char *data);
public slots:
    void newConnection();

    void readyRead();
    void disconnected();
    void incomingData(int size, char *data);
private:
    QTcpServer *mServer;
    QList<QTcpSocket*> mSockets;
    char buf[2000];
};

#endif // RAWSERVER_H
