#ifndef CORECLIENT_H
#define CORECLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include "rawclient.h"

class CoreClient : public QObject
{
    Q_OBJECT
public:
    explicit CoreClient(QObject *parent = 0);
    
signals:
    
public slots:
    void connected();
    void readyRead();
    void test();
    void incomingData(const QByteArray &data);
private:
    QTcpSocket *mMainSocket;
    quint64 mNextBlockSize;
    RawClient *mRawClient;
};

#endif // CORECLIENT_H
