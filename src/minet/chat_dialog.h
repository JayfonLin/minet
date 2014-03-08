#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>
#include <QtGui>
#include <QTimer>
#include <QtWidgets>

#include "connection.h"
#include "chat_log.h"
#include "p2pMessage.h"
#include "parseProtocol.h"
#include <QTcpSocket>

namespace Ui {
class Chat_dialog;
}
class Chat_dialog : public QDialog
{
    Q_OBJECT

public:
    Chat_dialog(QTcpSocket*, QString);
    Chat_dialog(QString, QString, quint16);
    ~Chat_dialog();

private:

    QTcpSocket *connection;					//Tcp Socket

    ParseProtocol *protocol;				//协议

    QTimer *timer;

    QString username;
    QString ip;
    quint16 port;

    QTextBrowser *displayMsg;				//显示消息框
    QTextEdit *sendMsg;						//发送消息框
    QPushButton *chatListBtn;				//消息记录按钮
    QPushButton *closeBtn;					//关闭窗口按钮
    QPushButton *sendBtn;					//发送信息按钮

    void initUi();													//初始化ui界面
    void initSignalsAndSlots();
    void readMessage();

private slots:
    void on_chatListBtn_clicked();					//消息记录按钮被按下
    void on_closeBtn_clicked();						//关闭按钮被按下
    void on_sendBtn_clicked();						//发送按钮被按下
    void hasConnected();							//已经和私聊的对方连接
    void readMsg();									//读取信息

    void timeUpDate();
};
#endif // CHAT_DIALOG_H
