#include "parseProtocol.h"

ParseProtocol::ParseProtocol()
{
}

ParseProtocol::~ParseProtocol()
{

}

int ParseProtocol::parseProtocol(QString protocol)
{
    protocolType = 0;

    QString requestLine = "";

    if(protocol == ""){
        return protocolType;
    }
    // 通过回车讲字符串分为若干部分放在列表
    QStringList lines = protocol.split(RETURN);

    requestLine = lines.at(0);										//协议的请求行

    QStringList requestLines = requestLine.split(SPACE);

    if(requestLines.at(0) != "" && requestLines.at(0) == MIRO){     //电政版本HELO没有CS版本号
        protocolType = 1;
    } else if(requestLines.at(1) != "" && requestLines.at(1) == STATUS){
        protocolType = 2;
    } else if(requestLines.at(1) != "" && requestLines.at(1) == LIST){
        protocolType = 3;
    } else if(requestLines.at(1) != "" && requestLines.at(1) == UPDATE){
        protocolType = 4;
    } else if(requestLines.at(1) != "" && requestLines.at(1) == CSMESSAGE){
        protocolType = 5;
    }

    return protocolType;
}

/**
 *
 *	发送HELO给服务器
 *
 */
QString ParseProtocol::sendHELOToHost(QString hostname)
{
    QString protocol = "";
    //protocol.append(CSVERSION);
    //protocol.append(SPACE);
    protocol.append(MINET);
    protocol.append(SPACE);
    protocol.append(hostname);
    protocol.append(RETURN);

    return protocol;
}

/**
 *
 *	解析服务器发来的HELO协议
 *
 */
bool ParseProtocol::parseHELOProtocol(QString protocol)
{
    QStringList requestLine = protocol.split(RETURN);

    QStringList lines = requestLine.at(0).split(SPACE);

    if(lines.at(1) != "" && lines.at(1) == MIRO){					//同样电政版本不同
        return true;
    }

    return false;
}

/**
 *
 *	构造一条能够发送到服务器的请求登陆协议
 *
 */
QString ParseProtocol::sendLOGINProtocol(QString username)
{
    QString protocol = "";

    //构造请求行
    QString requestLine = "";

    requestLine.append(CSVERSION);
    requestLine.append(SPACE);
    requestLine.append(LOGIN);
    requestLine.append(SPACE);
    requestLine.append(username);
    requestLine.append(RETURN);

    //构造首部行
    QString firstLines = "";

    if(this->firstLine.count() != 0){

        for(int i = 0; i < this->firstLine.count(); i++){

            map tmpMap = firstLine.at(i);
            firstLines.append(tmpMap.key);
            firstLines.append(SPACE);
            firstLines.append(tmpMap.value);
            firstLines.append(RETURN);
        }

    }

    //构造实体主体
    QString contentData = "";
    if(this->contentData != ""){

        contentData = this->contentData;
    }

    //构造登陆协议
    protocol.append(requestLine);
    protocol.append(firstLines);
    protocol.append(RETURN);
    protocol.append(contentData);

    return protocol;
}

/**
 *
 *	解析登陆LOGIN协议
 *
 */
bool ParseProtocol::parseLOGINProtocol(QString protocol)
{
    QString requestLine = "";
    QStringList lines1 = protocol.split(RETURN);

    requestLine = lines1.at(0);

    //解析首部行并把首部字段名和值保存到一个列表中
    for(int i = 1; i < lines1.count() - 2; i++){

        QStringList firstLine;
        firstLine = lines1.at(i).split(SPACE);

        map newMap;
        if(firstLine.at(0) != "" && firstLine.at(1) != ""){

            newMap.key = firstLine.at(0);
            newMap.value = firstLine.at(1);
        }

        this->firstLine.append(newMap);
    }

    //解析实体主体
    QString contentData = "";
    if(lines1.at(lines1.count() - 1) != ""){

        contentData = lines1.at(lines1.count() - 1);
    }
    this->contentData = contentData;

    if(requestLine.split(SPACE).at(2) == "1"){
        return true;
    }
    return false;
}

/**
 *
 *	返回一条GETLIST协议
 *
 */
QString ParseProtocol::sendGETLISTProtocol()
{
    QString protocol = "";

    //构造请求行
    QString requestLine = "";

    requestLine.append(CSVERSION);
    requestLine.append(SPACE);
    requestLine.append(GETLIST);
    requestLine.append(RETURN);

    //构造首部行
    QString firstLines = "";

    if(this->firstLine.count() != 0){

        for(int i = 0; i < this->firstLine.count(); i++){

            map tmpMap = firstLine.at(i);
            firstLines.append(tmpMap.key);
            firstLines.append(SPACE);
            firstLines.append(tmpMap.value);
            firstLines.append(RETURN);
        }

    }

    //构造实体主体
    QString contentData = "";
    if(this->contentData != ""){

        contentData = this->contentData;
    }

    //构造请求在线用户列表协议
    protocol.append(requestLine);
    protocol.append(firstLines);
    protocol.append(RETURN);
    protocol.append(contentData);

    return protocol;
}

/**
 *
 * 解析LIST协议
 *
 */
bool ParseProtocol::parseLISTProtocol(QString protocol)
{
    QString requestLine = "";
    QStringList lines1 = protocol.split(RETURN);

    requestLine = lines1.at(0);

    int i = 1;

    //解析首部行并把首部字段名和值保存到一个列表中
    for(; lines1.at(i) != ""; i++){

        QStringList firstLine;
        firstLine = lines1.at(i).split(SPACE);

        map newMap;
        if(firstLine.at(0) != "" && firstLine.at(1) != ""){

            newMap.key = firstLine.at(0);
            newMap.value = firstLine.at(1);
        }

        this->firstLine.append(newMap);
    }

    //去除空行
    ++i;

    //解析实体主体
    QString contentData = "";

    //因为可能重复请求LIST
    for(; i < lines1.count() - 1 && lines1.at(i) != ""; i++){
        contentData.append(lines1.at(i));
        contentData.append(RETURN);
    }
    this->contentData = contentData;

    if(this->contentData.length() != 0){
        return true;
    }

    return false;
}

/**
 *
 * 解析在线用户列表
 *
 */
void ParseProtocol::parseUserList()
{
    if(this->contentData.count() == 0){
        return;
    }

    QString data = "";
    QStringList lines = this->contentData.split(RETURN);

    for(int i = 0; i < lines.count(); i++){
        QStringList row = lines.at(i).split(SPACE);

        if(row.at(0) != "" && row.at(1) != "" && row.at(2) != ""){
            user newUser;
            newUser.username = row.at(0);
            newUser.ip = row.at(1);
            newUser.port = row.at(2);

            this->userlist.append(newUser);
        }
    }
}

/**
 *
 * 解析UPDATE协议
 *
 */
bool ParseProtocol::parseUPDATEProtocol(QString protocol)
{
    QString requestLine = "";
    QStringList lines1 = protocol.split(RETURN);

    requestLine = lines1.at(0);

    //解析首部行并把首部字段名和值保存到一个列表中
    for(int i = 1; i < lines1.count() - 2; i++){

        QStringList firstLine;
        firstLine = lines1.at(i).split(SPACE);

        map newMap;
        if(firstLine.at(0) != "" && firstLine.at(1) != ""){

            newMap.key = firstLine.at(0);
            newMap.value = firstLine.at(1);
        }

        this->firstLine.append(newMap);
    }

    //解析实体主体
    QString contentData = "";
    if(lines1.at(lines1.count() - 1) != ""){

        contentData = lines1.at(lines1.count() - 1);
    }
    this->contentData = contentData;

    return true;
}

/**
 *
 * 解析用户状态
 *
 */
QStringList ParseProtocol::parseUserStatus()
{
    QStringList userAndStatus;
    if(this->requestLine.count() == 0){
        return userAndStatus;
    }

    QString requestLine = "";
    QStringList row = this->requestLine.split(SPACE);


    if(row.at(1) == UPDATE && row.at(2) != "" && row.at(3) != ""){
        userAndStatus.append(row.at(2));
        userAndStatus.append(row.at(3));
    }

    return userAndStatus;
}

/**
 *
 * 解析群发消息CSMESSAGE协议
 *
 */
bool ParseProtocol::parseCSMESSAGEProtocol(QString protocol)
{
    QString requestLine = "";
    QStringList lines1 = protocol.split(RETURN);

    this->requestLine = lines1.at(0);

    //解析首部行并把首部字段名和值保存到一个列表中
    for(int i = 1; i < lines1.count() - 2; i++){

        QStringList firstLine;
        firstLine = lines1.at(i).split(SPACE);

        map newMap;
        if(firstLine.at(0) != "" && firstLine.at(1) != ""){

            newMap.key = firstLine.at(0);
            newMap.value = firstLine.at(1);
        }

        this->firstLine.append(newMap);
    }

    //解析实体主体
    QString contentData = "";
    if(lines1.at(lines1.count() - 1) != ""){

        contentData = lines1.at(lines1.count() - 1);
    }
    this->contentData = contentData;

    return true;
}

/**
 *
 * 构造一条群发消息协议
 *
 */
QString ParseProtocol::sendMESSAGEProtocol(QString username)
{
    QString protocol = "";

    //构造请求行
    QString requestLine = "";

    requestLine.append(CSVERSION);
    requestLine.append(SPACE);
    requestLine.append(MESSAGE);
    requestLine.append(SPACE);
    requestLine.append(username);
    requestLine.append(RETURN);

    //构造首部行
    QString firstLines = "";

    if(this->firstLine.count() != 0){

        for(int i = 0; i < this->firstLine.count(); i++){

            map tmpMap = firstLine.at(i);
            firstLines.append(tmpMap.key);
            firstLines.append(SPACE);
            firstLines.append(tmpMap.value);
            firstLines.append(RETURN);
        }

    }

    //构造实体主体
    QString contentData = "";
    if(this->contentData != ""){

        contentData = this->contentData;
    }

    //构造请求在线用户列表协议
    protocol.append(requestLine);
    protocol.append(firstLines);
    protocol.append(RETURN);
    protocol.append(contentData);

    return protocol;
}

/**
 *
 * 发送离线消息
 *
 */
QString ParseProtocol::sendLEAVEProtocol(QString username)
{
    QString protocol = "";

    //构造请求行
    QString requestLine = "";

    requestLine.append(CSVERSION);
    requestLine.append(SPACE);
    requestLine.append(LEAVE);
    requestLine.append(SPACE);
    requestLine.append(username);
    requestLine.append(RETURN);

    //构造首部行
    QString firstLines = "";

    if(this->firstLine.count() != 0){

        for(int i = 0; i < this->firstLine.count(); i++){

            map tmpMap = firstLine.at(i);
            firstLines.append(tmpMap.key);
            firstLines.append(SPACE);
            firstLines.append(tmpMap.value);
            firstLines.append(RETURN);
        }

    }

    //构造实体主体
    QString contentData = "";
    if(this->contentData != ""){

        contentData = this->contentData;
    }

    //构造请求在线用户列表协议
    protocol.append(requestLine);
    protocol.append(firstLines);
    protocol.append(RETURN);
    protocol.append(contentData);

    return protocol;
}

QString ParseProtocol::sendBEATProtocol(QString username)
{
    QString protocol = "";

    //构造请求行
    QString requestLine = "";

    requestLine.append(CSVERSION);
    requestLine.append(SPACE);
    requestLine.append(BEAT);
    requestLine.append(SPACE);
    requestLine.append(username);
    requestLine.append(RETURN);

    //构造首部行
    QString firstLines = "";

    if(this->firstLine.count() != 0){

        for(int i = 0; i < this->firstLine.count(); i++){

            map tmpMap = firstLine.at(i);
            firstLines.append(tmpMap.key);
            firstLines.append(SPACE);
            firstLines.append(tmpMap.value);
            firstLines.append(RETURN);
        }

    }

    //构造实体主体
    QString contentData = "";
    if(this->contentData != ""){

        contentData = this->contentData;
    }

    //构造请求在线用户列表协议
    protocol.append(requestLine);
    protocol.append(firstLines);
    protocol.append(RETURN);
    protocol.append(contentData);

    return protocol;
}

bool ParseProtocol::parseP2PMessageProtocol(QString protocol)
{
    QString requestLine = "";
    QStringList lines1 = protocol.split(RETURN);

    requestLine = lines1.at(0);

    //解析首部行并把首部字段名和值保存到一个列表中
    for(int i = 1; i < lines1.count() - 2; i++){

        QStringList firstLine;
        firstLine = lines1.at(i).split(SPACE);

        map newMap;
        if(firstLine.at(0) != "" && firstLine.at(1) != ""){

            newMap.key = firstLine.at(0);
            newMap.value = firstLine.at(1);
        }

        this->firstLine.append(newMap);
    }

    //解析实体主体
    QString contentData = "";
    if(lines1.at(lines1.count() - 1) != ""){

        contentData = lines1.at(lines1.count() - 1);
    }
    this->contentData = contentData;

    return true;
}

QString ParseProtocol::sendP2PMessageProtocol(QString username)
{
    QString protocol = "";

    //构造请求行
    QString requestLine = "";

    requestLine.append(P2PVERSION);
    requestLine.append(SPACE);
    requestLine.append(P2PMESSAGE);
    requestLine.append(SPACE);
    requestLine.append(username);
    requestLine.append(RETURN);

    //构造首部行
    QString firstLines = "";

    if(this->firstLine.count() != 0){

        for(int i = 0; i < this->firstLine.count(); i++){

            map tmpMap = firstLine.at(i);
            firstLines.append(tmpMap.key);
            firstLines.append(SPACE);
            firstLines.append(tmpMap.value);
            firstLines.append(RETURN);
        }

    }

    //构造实体主体
    QString contentData = "";
    if(this->contentData != ""){

        contentData = this->contentData;
    }

    //构造登陆协议
    protocol.append(requestLine);
    protocol.append(firstLines);
    protocol.append(RETURN);
    protocol.append(contentData);

    return protocol;
}

/**
 *
 * 得到群发消息的用户名
 *
 */
QString ParseProtocol::getCsMessageUsername()
{
    QString username;
    if(this->requestLine != ""){
        QStringList row = this->requestLine.split(SPACE);
        if(row.at(1) == CSMESSAGE && row.at(2) != ""){
            username = row.at(2);
        }
    }

    return username;
}

/**
 *
 *	根据客户端传来的首部字段名和值在当前的首部行中增加一条记录
 *
 */
void ParseProtocol::addAMap(QString key, QString value)
{
    QList<map> firstLine;

    map newMap;
    newMap.key = key;
    newMap.value = value;

    firstLine.append(newMap);

    this->firstLine.append(firstLine);
}

/**
 *
 *	增加实体主体
 *
 */
void ParseProtocol::addContentData(QString contentData)
{
    this->contentData = contentData;
}

QString ParseProtocol::getContentData()
{
    return this->contentData;
}

QList<user> ParseProtocol::getUserList()
{
    return this->userlist;
}
