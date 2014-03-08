#include "p2pMessage.h"

P2PMessage::P2PMessage(QString message)
{
    this->message = message;
    firstLine = QMap<QString, QString>();
}

QString P2PMessage::getMessage()
{
    return this->message;
}

QString P2PMessage::getP2PVersion()
{
    return this->p2pVersion;
}

QString P2PMessage::getP2PMessage()
{
    return this->p2pMessage;
}

QString P2PMessage::getContentData()
{
    return this->contentData;
}

QMap<QString, QString> P2PMessage::getFirstLine()
{
    return this->firstLine;
}


void P2PMessage::setMessage(QString message)
{
    this->message = message;
}

void P2PMessage::setP2PVersion(QString p2pVersion)
{
    this->p2pVersion = p2pVersion;
}

void P2PMessage::setP2PMessage(QString p2pMessage)
{
    this->p2pMessage = p2pMessage;
}

void P2PMessage::setContentData(QString contentData)
{
    this->contentData = contentData;
}

void P2PMessage::setFirstLine(QMap<QString, QString> firstLine)
{
    this->firstLine = firstLine;
}

// 解析P2P协议
int P2PMessage::parseP2PProtocol()
{
    //int protocolType = 0;
    const QString RETURN = "\r\n";
    const QString SPACE = " ";

    QString message = this->message;

    QStringList messageBlock = message.split(RETURN);

    int blockCount = messageBlock.count();

    //对请求行的处理
    QStringList requestRow = messageBlock.at(0).split(SPACE);

    if(requestRow.count() == 3){
        this->p2pVersion = requestRow.at(0);
        this->p2pMessage = requestRow.at(1);
        this->username = requestRow.at(2);
    } else {

        return REQUEST_ERROR;
    }
    //对首部行的处理
    for(int i = 1; i < blockCount - 2; i++){

        QStringList dataStrArr = messageBlock.at(i).split(SPACE);
        firstLine.insert(dataStrArr.at(0), dataStrArr.at(1));
    }

    this->parseFirstLine();
    //对实体主体的处理
    contentData = messageBlock.at(blockCount - 1);
    return 1;
}


void P2PMessage::parseFirstLine()
{
}

P2PMessage::~P2PMessage()
{
    this->message = "";
}
