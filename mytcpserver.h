#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QCoreApplication>
#include <QString>

class MyTcpServer: public QObject
{
public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();
private:
    static inline MyTcpServer *p_instance = nullptr;
    MyTcpServer(QObject *parent= nullptr) : QObject(parent)
    {
        tcpServer = new QTcpServer(this);
        connect(tcpServer, &QTcpServer::newConnection,
                this, &MyTcpServer::slotNewConnection);

        if(!tcpServer->listen(QHostAddress::Any, 33333)){
            qDebug() << "Сервер не запущен";
        } else {
            serverStatus = 1;
            qDebug() << "Сервер запущен";
        }
    }
    MyTcpServer(const MyTcpServer& );
    MyTcpServer& operator = (MyTcpServer &);

    QTcpServer* tcpServer;
    int serverStatus;
    QMap<int, QTcpSocket*> tcpSocket;
public:
    static MyTcpServer* getInstance();
};

#endif // TCPSERVER_H
