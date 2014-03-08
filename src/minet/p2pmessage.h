#ifndef P2PMESSAGE_H
#define P2PMESSAGE_H

#include <QString>
#include <QStringList>
#include <QMap>

enum P2PMessageERROR{
    REQUEST_ERROR
};

class P2PMessage
{
public:
    P2PMessage(QString);
    ~P2PMessage();

    QString getMessage();
    QString getP2PVersion();
    QString getP2PMessage();
    QString getUsername();
    QString getContentData();
    QMap<QString, QString> getFirstLine();

    void setMessage(QString);
    void setP2PVersion(QString);
    void setP2PMessage(QString);
    void setUsername(QString);
    void setContentData(QString);
    void setFirstLine(QMap<QString, QString>);

    int parseP2PProtocol();

private:
    QString message;					//请求报文
    QString p2pVersion;					//P2P协议版本
    QString p2pMessage;					//P2PMessage
    QString username;					//用户名
    QString contentData;				//信息主体
    QMap<QString, QString> firstLine;	//首部行

    void parseFirstLine();				//解析首部行
};

#endif
