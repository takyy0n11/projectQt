#include "mytcpserver.h"
#include "functions.h"

void MyTcpServer::slotNewConnection()
{
    QTcpSocket* socket = tcpServer->nextPendingConnection();
    if(serverStatus==1){
        socket->write("Добро пожаловать на сервер!\r\n");
        connect(socket, &QTcpSocket::readyRead,
                this,&MyTcpServer::slotServerRead);
        connect(socket,&QTcpSocket::disconnected,
                this,&MyTcpServer::slotClientDisconnected);
    }
    tcpSocket.insert(socket->socketDescriptor(), socket);
}

MyTcpServer* MyTcpServer::getInstance()
{
    if (!p_instance)
        p_instance = new MyTcpServer();
    return p_instance;
}

void MyTcpServer::slotClientDisconnected()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->close();
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    QString data;
    QByteArray response;
    while(socket->bytesAvailable()>0) {
        data.append(socket->readLine());
    }
    if (data.size() > 0 && data.back() == '\n') {
        QStringList datas = data.split("\n");
        datas.removeLast();
        for (int i = 0; i < datas.size(); i++) {
            response = Functions::getInstance()->parseUserMessage(data);
            socket->write(response + '\n');
            data.clear();
        }
    }
}

