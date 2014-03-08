#include "connection.h"

Connection::Connection(QObject *parent)
    : QTcpSocket(parent)
{

}

void Connection::newConnection(QString ipAddress, quint16 port)
{
    this->connectToHost(ipAddress, port);
}

QString Connection::readMsg()
{
    //QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //this->waitForConnected();
    QByteArray qba = this->readAll();

    QString str = QVariant(qba).toString();

    //QHostAddress ip = this->peerAddress();
    //QString sendInfo = ip.toString() + " : " + time + "\n" + str;

    return str;
}

void Connection::sendMsg(QString message)
{
    this->write(message.toStdString().c_str(), strlen(message.toStdString().c_str()));
}

Connection::~Connection()
{
}
