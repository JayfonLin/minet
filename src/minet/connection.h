#ifndef CONNECT_H
#define CONNECT_H

#include <QtNetwork>
#include <QtGui>
#include "parseProtocol.h"
#include <QtWidgets>
class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    Connection(QObject *parent = 0);
    ~Connection();

    void newConnection(QString, quint16);

    QString readMsg();
    void sendMsg(QString);

};

#endif
