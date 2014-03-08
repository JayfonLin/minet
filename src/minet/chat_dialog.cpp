#include "chat_dialog.h"

Chat_dialog::Chat_dialog(QTcpSocket *conn, QString username)
{
    this->connection = conn;
    this->username = username;
    this->port = 5555;
    this->ip = "192.168.1.108";
    this->initUi();
    this->initSignalsAndSlots();
}

Chat_dialog::Chat_dialog(QString username, QString hostIPAddress, quint16 port)
{
    this->username = username;
    this->ip = ip;
    this->port = port;

    connection = new QTcpSocket();
    connection->connectToHost(hostIPAddress, port);
    this->initUi();
    this->initSignalsAndSlots();
}

// UI初始化
void Chat_dialog::initUi()
{
    //添加最小化按钮
    Qt::WindowFlags flags=Qt::Dialog;
    flags |= Qt::WindowMinimizeButtonHint;
    this->setWindowFlags(flags);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    //初始化显示消息框
    displayMsg = new QTextBrowser;
    //displayMsg->setMaximumSize(400, 220);
    displayMsg->setMinimumSize(400, 220);

    QHBoxLayout *chatLogLayout = new QHBoxLayout;

    //消息记录按钮
    chatListBtn = new QPushButton();
    chatListBtn->setText(tr("chat log"));
    chatListBtn->setMaximumSize(80, 30);
    //chatListBtn->setAutoRaise(true);

    chatLogLayout->addWidget(chatListBtn, 0, Qt::AlignLeft);

    //初始化发送信息框
    sendMsg = new QTextEdit;

    sendMsg->setMaximumSize(520, 120);
    sendMsg->setMinimumSize(520, 120);

    QHBoxLayout *btnLayout = new QHBoxLayout;

    //初始化关闭按钮，发送按钮
    closeBtn = new QPushButton(tr("close"));
    sendBtn = new QPushButton(tr("send"));
    closeBtn->setMinimumHeight(23);
    sendBtn->setMaximumHeight(23);

    btnLayout->addSpacing(350);
    btnLayout->addWidget(closeBtn, Qt::AlignRight);
    btnLayout->addWidget(sendBtn, Qt::AlignRight);

    //将空间添加到主布局上
    mainLayout->addWidget(displayMsg);
    mainLayout->addLayout(chatLogLayout);
    mainLayout->addWidget(sendMsg);
    mainLayout->addLayout(btnLayout);

    this->setMinimumSize(400, 220);
    //this->setMaximumSize(400, 520);
    setLayout(mainLayout);
}

// 信号槽
void Chat_dialog::initSignalsAndSlots()
{
    this->connect(chatListBtn, SIGNAL(clicked()), this, SLOT(on_chatListBtn_clicked()));
    this->connect(closeBtn, SIGNAL(clicked()), this, SLOT(on_closeBtn_clicked()));
    this->connect(sendBtn, SIGNAL(clicked()), this, SLOT(on_sendBtn_clicked()));
    this->connect(connection, SIGNAL(connected()), this, SLOT(hasConnected()));
    this->connect(connection, SIGNAL(readyRead()), this, SLOT(readMsg()));
}


// 查看聊天记录
void Chat_dialog::on_chatListBtn_clicked()
{
    QString chatLog = displayMsg->toPlainText();
    Chat_log *chat_log = new Chat_log(chatLog);
    chat_log->show();
    chat_log->exec();
}

// 关闭窗口
void Chat_dialog::on_closeBtn_clicked()
{
    this->close();
}

// 发送私聊信息
void Chat_dialog::on_sendBtn_clicked()
{

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    this->protocol = new ParseProtocol;
    this->protocol->addContentData(sendMsg->toPlainText());
    QString protocol = this->protocol->sendP2PMessageProtocol(username);

    QByteArray tmp = protocol.toStdString().c_str();
    connection->write(tmp, strlen(tmp));
   // connection->sendMsg(protocol);

    QString sendInfo = "Me : " + time + "\n" + sendMsg->toPlainText();
    sendMsg->clear();
    displayMsg->append(sendInfo);
    displayMsg->append("\n");
}

// 读取发送的信息
void Chat_dialog::readMsg()
{
    QByteArray tmp = connection->readAll();
    QString message = QVariant(tmp).toString();

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //displayMsg->append(message);
    P2PMessage *p2p = new P2PMessage(message);
    int a = p2p->parseP2PProtocol();
   // this->displayFontSetting(p2p);
    displayMsg->append(this->username + " : " + time + "\n");
    displayMsg->append(p2p->getContentData());
    displayMsg->append("\n");
}



void Chat_dialog::hasConnected()
{
    QMessageBox::information(this, tr("Connect successfully!"), tr("Has connected to the client"));
    protocol = new ParseProtocol;

    protocol->sendHELOToHost(username);

    timer = new QTimer(this);
    this->connect(timer, SIGNAL(timeout()), this, SLOT(timeUpDate()));
    timer->start(6000);
}

// 心跳信息
void Chat_dialog::timeUpDate()
{
    protocol = new ParseProtocol;
    QString protocol = this->protocol->sendBEATProtocol(username);
    QByteArray tmp = protocol.toStdString().c_str();
    //connection->write(tmp, strlen(tmp));
    //this->connection->sendMsg(protocol);
}

Chat_dialog::~Chat_dialog()
{
    this->connection->close();
    delete this->closeBtn;
    delete this->sendBtn;
}
