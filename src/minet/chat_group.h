#ifndef CHAT_GROUP_H
#define CHAT_GROUP_H

#include <QDialog>
#include <QtGui>
#include <QTimer>
#include "connection.h"
#include "chat_log.h"
#include "chat_dialog.h"
#include "macro.h"
#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class Chat_group;
}

class Chat_group : public QDialog
{
    Q_OBJECT

public:
    Chat_group();
    ~Chat_group();

private:

    QTcpSocket *serverConnect;       // 收到私聊信息时的socket连接
    QTcpServer *serverListen;       // 监听端口
    Connection *connection;						//Tcp Socket
    ParseProtocol *protocol;
    QTimer *timer;

    //登陆界面
    QLineEdit *hostIPAddress;					//输入主机ip地址
    QLineEdit *port;							//输入端口号
    QLineEdit *username;						//输入用户名
    QPushButton *connectBtn;					//连接按钮

    //群聊界面
    QTextBrowser *displayMsg;				//显示消息框
    QTextEdit *sendMsg;						//发送消息框
    QPushButton *chatListBtn;				//消息记录按钮
    QPushButton *closeBtn;					//关闭窗口按钮
    QPushButton *sendBtn;					//发送信息按钮
    QTableWidget *userList;					//在线用户列表
    QLabel *userCount;						//在线人数
    QPushButton *refreshUserListBtn;		//手动更新在线人数列表按钮
    void closeEvent(QCloseEvent *event);

    void initUi();													//初始化ui界面
    void initSignalsAndSlots();										//初始化信号和槽
    void reInitButtons();											//登陆失败，重新初始化按钮
    bool isTheIpAddressValid();										//判断ip地址是否是正确的
    void sendHeloToHost();											//发送HELO到服务器
    void sendLoginProtocol();										//发送登陆协议
    void initTableWidget();											//初始化在线用户列表
    void updateAUserStatus(QStringList);							//更新用户状态
    void getCSMessage(QString, QString);							//收到群发消息，在文本框显示
    bool hasTheUser(QString);										//是否有这个用户
    void sendLeaveToHost();											//发送离线信息给服务器
    void newParticipant(QString username, QString ip, QString port);
    void p2pListen();               //监听p2p连接

    void dealMessage();

private slots:
    void connectToHost();							//连接到服务器
    void hasConnected();							//已经建立好连接
    void breakConnection();							//断开连接
    void readProtocol();							//读取服务区发来的协议
    void enableConnectBtn();						//使连接按钮可用
    void refreshUserList(QList<user>);				//刷新用户列表
    void on_clicked_onlineUserList();				//请求在线用户列表

    void on_chatListBtn_clicked();					//消息记录按钮被按下
    void on_closeBtn_clicked();						//关闭按钮被按下
    void on_sendBtn_clicked();						//发送按钮被按下
    void returnPressed();							//按下确定按钮后，发送消息
    void timeUpDate();
    void getItem(QTableWidgetItem*);

    void acceptConnection();
};

#endif // CHAT_GROUP_H
