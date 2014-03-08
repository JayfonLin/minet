#ifndef PARSEPROTOCOL_H
#define PARSEPROTOCOL_H

#include <QString>
#include <QStringList>
#include <QList>
#include "macro.h"

struct map{

    QString key;
    QString value;
};

struct user{
    QString username;
    QString ip;
    QString port;
};

class ParseProtocol
{
public:
    ParseProtocol();
    ~ParseProtocol();

    int parseProtocol(QString);
    QString sendHELOToHost(QString);			//发送HELO协议报文
    bool parseHELOProtocol(QString);			//解析HELO协议报文

    QString sendLOGINProtocol(QString);			//发送LOGIN协议报文
    bool parseLOGINProtocol(QString);			//解析LOGIN协议报文

    QString sendGETLISTProtocol();				//发送请求在线名单GETLIST
    bool parseLISTProtocol(QString);			//解析用户发在线用户名单LIST

    bool parseUPDATEProtocol(QString);			//解析UPDATE协议

    bool parseCSMESSAGEProtocol(QString);		//解析CSMESSAGE协议
    QString sendMESSAGEProtocol(QString);		//发送群发信息

    QString sendLEAVEProtocol(QString);			//发送离线信息

    QString sendBEATProtocol(QString);			//发送心跳信息

    bool parseP2PMessageProtocol(QString);		//解析私聊信息
    QString sendP2PMessageProtocol(QString);	//发送私聊信息

    void addAMap(QString, QString);				//合成一条map信息，包含字段名和值
    void addContentData(QString);				//在协议中加入主体实体内容
    QString getContentData();					//获得实体主体

    void parseUserList();						//解析用户列表
    QStringList parseUserStatus();				//解析用户登录或下线状态
    QString getCsMessageUsername();				//获得群发消息的用户名
    QList<user> getUserList();					//获得在线用户列表

private:
    int protocolType;
    QString requestLine;
    QList<map> firstLine;						//包含键值的首部行
    QString contentData;						//实体主体
    QList<user>	userlist;						//用户列表


};

#endif
