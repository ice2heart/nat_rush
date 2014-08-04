#ifndef CORESERVER_H
#define CORESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QMap>
#include "rawserver.h"

class CoreServer : public QObject
{
    Q_OBJECT
public:
    explicit CoreServer(QObject *parent = 0);
    
signals:

    
public slots:
    void newConnection();

    void readyRead();
    void disconnected();
    void sendText(QTcpSocket *socket, const QString &text);
    void sendRawData(QTcpSocket *socket, int bufsize, char *buf);
    void incomingRawData(int size, char *data);

private:
    QTcpServer *mMainServer;
    QMap<QTcpSocket*, quint64> mCoreClients;
    RawServer *mRawServer;
};

#endif // CORESERVER_H
