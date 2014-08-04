#ifndef RAWCLIENT_H
#define RAWCLIENT_H

#include <QObject>
#include <QTcpSocket>

class RawClient : public QObject
{
    Q_OBJECT
public:
    explicit RawClient(QObject *parent = 0);
    
signals:
    void newData(char *data, int len);
public slots:
    void connected();
    void readyRead();
    void sendRawData(char *data, int len);
private:
    QTcpSocket *mSocket;
    char buf[2000];
};

#endif // RAWCLIENT_H
