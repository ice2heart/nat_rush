#ifndef RAWCLIENT_H
#define RAWCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <common.h>

class RawClient : public QObject
{
    Q_OBJECT
public:
    explicit RawClient(QObject *parent = 0);
    
signals:
    void newData(const QByteArray &data);
public slots:
    void connected();
    void readyRead();
    void sendRawData(const QByteArray &data);
private:
    QTcpSocket *mSocket;
    char buf[BUFSIZE];
};

#endif // RAWCLIENT_H
