#include "chat_group.h"
#include <QWidget>

Chat_group::Chat_group()
{
    connection = new Connection(this);
    connection->abort();

    this->initUi();
    this->initSignalsAndSlots();
}

// 初始化界面
void Chat_group::initUi()
{
    //添加最小化按钮
    Qt::WindowFlags flags=Qt::Dialog;
    flags |= Qt::WindowMinimizeButtonHint;
    this->setWindowFlags(flags);


    QHBoxLayout *mainLayout = new QHBoxLayout;

    QVBoxLayout *layoutLeft = new QVBoxLayout;

    //初始化显示消息框
    displayMsg = new QTextBrowser;
    displayMsg->setMaximumSize(350, 270);
    displayMsg->setMinimumSize(300, 270);

    QHBoxLayout *historyLayout = new QHBoxLayout;
    //消息记录按钮
    chatListBtn = new QPushButton();
    chatListBtn->setText(tr("Chat History"));
    chatListBtn->setMinimumSize(31, 30);
    //chatListBtn->setAutoRaise(true);
    historyLayout->addWidget(chatListBtn, 0, Qt::AlignRight);

    //初始化发送信息框
    sendMsg = new QTextEdit;

    sendMsg->setMaximumSize(400, 270);

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
    layoutLeft->addWidget(displayMsg);
    //layoutLeft->addLayout(toolsLayout);
    layoutLeft->addLayout(historyLayout);
    layoutLeft->addWidget(sendMsg);
    layoutLeft->addLayout(btnLayout);

    //群聊窗口的右边为垂直布局
    QVBoxLayout *layoutRight = new QVBoxLayout;

    //在线用户列表
    userList = new QTableWidget;

    this->initTableWidget();


    //在线人数
    QHBoxLayout *onlineLayout = new QHBoxLayout;
    userCount = new QLabel(tr("online user count is:"));
    onlineLayout->addWidget(userCount);

    //手动刷新在线用户列表
    refreshUserListBtn = new QPushButton;
    refreshUserListBtn->setText(tr("refresh"));
    refreshUserListBtn->setMaximumSize(60, 40);


    onlineLayout->addWidget(refreshUserListBtn);

    layoutRight->addLayout(onlineLayout);

    //登陆界面

    //读取用户配置中的用户名，主机ip地址和端口号，避免用户重新输入
    //setting = new User_setting;
    QString usernameStr = "";//setting->getUsername();
    QString ipAddressStr = "";//setting->getHostIPAddress();
    QString portStr = "";//setting->getPort();
    bool hasUserSetting = "";//setting->hasUserSetting();

    QFormLayout *loginLayout = new QFormLayout;

    QFont font;
    font.setPointSize(10);

    //第一行， ip地址
    QLabel *ipLabel = new QLabel(tr("IP Address:"));
    ipLabel->setFont(font);

    hostIPAddress = new QLineEdit(ipAddressStr);
    hostIPAddress->setMinimumSize(180, 25);

    //第二行，端口号

    QLabel *portLabel = new QLabel(tr("Port:"));
    portLabel->setFont(font);

    port = new QLineEdit(portStr);
    port->setMinimumSize(180, 25);
    port->setValidator(new QIntValidator(1, 65535, this));

    //第三行，用户名
    QLabel *usernameLabel = new QLabel("Username:");
    usernameLabel->setFont(font);

    username = new QLineEdit(usernameStr);
    username->setMinimumSize(180, 25);

    loginLayout->addRow(ipLabel, hostIPAddress);
    loginLayout->addRow(portLabel, port);
    loginLayout->addRow(usernameLabel, username);

    //初始化连接按钮
    QHBoxLayout *btnLayout2 = new QHBoxLayout;

    connectBtn = new QPushButton(tr("connect"));
    connectBtn->setMaximumWidth(100);
    //connectBtn->setEnabled(hasUserSetting);

    btnLayout2->addSpacing(105);
    btnLayout2->addWidget(connectBtn, 0, Qt::AlignRight);

    layoutRight->addLayout(loginLayout);
    layoutRight->addLayout(btnLayout2);
    layoutRight->addWidget(userList);

    mainLayout->addLayout(layoutRight);
    mainLayout->addLayout(layoutLeft);

    this->setMinimumSize(400, 400);
    this->setMaximumSize(600, 600);
    setLayout(mainLayout);
}

// 初始化在线人员列表
void Chat_group::initTableWidget()
{
    this->userList->setMinimumSize(280, 300);
    if(this->userList->columnCount() < 3){
        this->userList->setColumnCount(3);
    }

    QHeaderView *headerView = userList->verticalHeader();
    headerView->setHidden(true);

    QStringList headerLabel;
    headerLabel.append("Username");
    headerLabel.append("IP");
    headerLabel.append("Port");
    this->userList->setHorizontalHeaderLabels(headerLabel);

    this->userList->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->userList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// 新来用户时更新用户
void Chat_group::newParticipant(QString username, QString ip, QString port)
{
    QTableWidgetItem *Username = new QTableWidgetItem(username);
    QTableWidgetItem *IP = new QTableWidgetItem(ip);
    QTableWidgetItem *Port = new QTableWidgetItem(port);

    int rows = this->userList->rowCount();
    this->userList->insertRow(rows);

    this->userList->setItem(rows, 0, Username);
    this->userList->setItem(rows, 1, IP);
    this->userList->setItem(rows, 2, Port);
}

// 初始化信号-槽
void Chat_group::initSignalsAndSlots()
{
    //登陆界面的信号和槽
    this->connect(connectBtn, SIGNAL(clicked()), this, SLOT(connectToHost()));
    this->connect(connection, SIGNAL(connected()), this, SLOT(hasConnected()));
    //The readyRead() signal is emitted every time a new chunk of data has arrived.
    this->connect(connection, SIGNAL(readyRead()), this, SLOT(readProtocol()));
    this->connect(connection, SIGNAL(disconnected()), this, SLOT(breakConnection()));

    //聊天界面的信号和槽
    this->connect(chatListBtn, SIGNAL(clicked()), this, SLOT(on_chatListBtn_clicked()));
    this->connect(closeBtn, SIGNAL(clicked()), this, SLOT(on_closeBtn_clicked()));
    this->connect(sendBtn, SIGNAL(clicked()), this, SLOT(on_sendBtn_clicked()));
    this->connect(sendMsg, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    this->connect(connection, SIGNAL(readyRead()), this, SLOT(readMsg()));
    this->connect(refreshUserListBtn, SIGNAL(clicked()), this, SLOT(on_clicked_onlineUserList()));
   // this->connect(settingBtn, SIGNAL(clicked()), this, SLOT(on_clicked_settingBtn()));
    this->connect(userList,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(getItem(QTableWidgetItem*)));

}



// 连接至服务器
void Chat_group::connectToHost()
{
    protocol = new ParseProtocol;

    if(this->isTheIpAddressValid()){

        QMessageBox::information(this, tr("Error"),
                                 tr("The ip address is not valid, please check it out!"));
    }

    connection->newConnection(this->hostIPAddress->text(), LISTENPORT);
}

// 判断IP地址是否合法
bool Chat_group::isTheIpAddressValid()
{
    QString ipStr = hostIPAddress->text();
    QRegExp ipReg("^(25[0-5] | 2[0-4][0-9] | [0-1]{1}[0-9]{2} | [1-9]{1}[0-9]{1} | [1-9])\\."
                  "(25[0-5] | 2[0-4][0-9] | [0-1]{1}[0-9]{2} | [1-9]{1}[0-9]{1} | [1-9] | 0)\\."
                  "(25[0-5] | 2[0-4][0-9] | [0-1]{1}[0-9]{2} | [1-9]{1}[0-9]{1} | [1-9] | 0)\\."
                  "(25[0-5] | 2[0-4][0-9] | [0-1]{1}[0-9]{2} | [1-9]{1}[0-9]{1} | [0-9])$");

    if(ipReg.indexIn(ipStr) == -1){

        return false;
    }

    return true;
}


void Chat_group::enableConnectBtn()
{
    //connectBtn->setEnabled(false);
    connectBtn->setEnabled(!this->hostIPAddress->text().isEmpty()
        && !this->port->text().isEmpty() &&!this->username->text().isEmpty());
}


/*
读取协议，使用flag来确定协议类型
flag为1，收到的是HELO协议
flag为2，收到的是LOGIN协议
flag为3，收到的LIST协议
flag为4， 收到的是UPDATE协议
flag为5， 收到的是群发消息协议

*/
void Chat_group::readProtocol()
{
    protocol = new ParseProtocol;
    QString protocolStr = connection->readMsg();
    //QMessageBox::information(this, tr("hello"), protocolStr);              //测试
    int flag = protocol->parseProtocol(protocolStr);


    if(flag == 1){
            this->sendLoginProtocol();
    } else if(flag == 2){

        if(protocol->parseLOGINProtocol(protocolStr)){
            QMessageBox::information(this, tr("Login"),
                "Login succeeds");
            timer = new QTimer(this);
            this->connect(timer, SIGNAL(timeout()), this, SLOT(timeUpDate()));
            timer->start(6000);
        } else {
            QMessageBox::information(this, tr("Login failed"), protocol->getContentData());
            this->reInitButtons();
            return;
        }
        this->on_clicked_onlineUserList();
    } else if(flag == 3){

        if(protocol->parseLISTProtocol(protocolStr)){
            protocol->parseUserList();
            QList<user> userList = protocol->getUserList();
            this->refreshUserList(userList);
        } else {
            QMessageBox::information(this, tr("Get online list failed"), "You can try it later!");
        }
    } else if(flag == 4){

        if(protocol->parseUPDATEProtocol(protocolStr)){
            QStringList userAndStatus = protocol->parseUserStatus();
            this->updateAUserStatus(userAndStatus);
        } else {
            QMessageBox::information(this, tr("Get User status failed"), "You can try it later!");
        }
    } else if(flag == 5){

        if(protocol->parseCSMESSAGEProtocol(protocolStr)){
            QString username = protocol->getCsMessageUsername();
            QString contentData = protocol->getContentData();
            this->getCSMessage(username, contentData);
        }
    }
}

// 发送HELO
void Chat_group::hasConnected()
{
    this->sendHeloToHost();
    this->connectBtn->setEnabled(false);
    this->hostIPAddress->setEnabled(false);
    this->port->setEnabled(false);
    this->username->setEnabled(false);

    // 确认连接上服务器后开启p2p监听
    this->p2pListen();

}

// p2p监听
void Chat_group::p2pListen()
{
    serverListen = new QTcpServer(this);
    printf("Listening...\n");
    // 监听端口
    serverListen->listen(QHostAddress::Any, port->text().toInt());
    printf("Listening port: %d\n", this->port->text().toInt());
    connect(serverListen,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

}

// 接受p2p请求
void Chat_group::acceptConnection()
{
    printf("%d\n", serverListen->isListening());
    QMessageBox *tmp1 = new QMessageBox();
    tmp1->setText("accept p2p request");
    tmp1->show();
    tmp1->exec();
    serverConnect = serverListen->nextPendingConnection();
    Chat_dialog *privateChat = new Chat_dialog(serverConnect, this->username->text());
    //privateChat->show();
    privateChat->exec();
    serverListen->close();
}

// 接受p2p请求开启私聊框
void Chat_group::dealMessage()
{
   // Chat_dialog *privateChat = new Chat_dialog(serverConnect);
}

// 断开服务器
void Chat_group::breakConnection()
{
    this->connectBtn->setEnabled(true);
    this->username->setEnabled(true);
    this->hostIPAddress->setEnabled(true);
    this->port->setEnabled(true);
}


void Chat_group::reInitButtons()
{
    this->connectBtn->setEnabled(true);
    this->username->setEnabled(true);
    this->hostIPAddress->setEnabled(true);
    this->port->setEnabled(true);
}

// 发送HELO协议
void Chat_group::sendHeloToHost()
{
    QString hostname = "127.0.0.1";												//要改为真正的hostname
    QString protocol = this->protocol->sendHELOToHost(hostname);
    connection->sendMsg(protocol);
}

// 发送LOGIN协议
void Chat_group::sendLoginProtocol()
{
    this->protocol = new ParseProtocol;
    QString username = this->username->text();
    QString port = this->port->text();
    protocol->addAMap("Port", port);
    protocol->addAMap("Content-Length:", "1333");
    //protocol->addContentData("Hello");
    QString protocol = this->protocol->sendLOGINProtocol(username);
    connection->sendMsg(protocol);
}

// 心跳信息
void Chat_group::timeUpDate()
{
    protocol = new ParseProtocol;
    QString protocol = this->protocol->sendBEATProtocol(this->username->text());
    this->connection->sendMsg(protocol);
}


void Chat_group::on_clicked_onlineUserList()
{
    this->protocol = new ParseProtocol;
    QString protocol = this->protocol->sendGETLISTProtocol();
    connection->sendMsg(protocol);
}

// 刷新列表
void Chat_group::refreshUserList(QList<user> userList)
{
    this->initTableWidget();

    for(int i = 0; i < userList.count(); i++){
        user tmpUser;
        tmpUser = userList.at(i);

        QString username = tmpUser.username;
        QString ip = tmpUser.ip;
        QString port = tmpUser.port;

        if(this->hasTheUser(username)){
            this->newParticipant(username, ip, port);
        }
    }
    //刷新在线用户数量
    QString onlineCountStr = "Online User count is:";
    int count = userList.count();
    QString onlineCountStr2 = onlineCountStr2.number(count);
    this->userCount->setText(onlineCountStr.append(onlineCountStr2));
}


bool Chat_group::hasTheUser(QString username)
{
    bool bb = this->userList->findItems(username,Qt::MatchExactly).isEmpty();
    return bb;
}


void Chat_group::updateAUserStatus(QStringList userAndStatus)
{
    if(userAndStatus.count() <= 0){
        return;
    }

    if(userAndStatus.at(0) != "" && userAndStatus.at(1) != ""){
        QString username = userAndStatus.at(1);
        QString status = userAndStatus.at(0);
    }
}

// 接受群发信息
void Chat_group::getCSMessage(QString username, QString contentData)
{

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    displayMsg->append(username + " " + time + "\r\n" + contentData + "\r\n");
}


// 返回聊天记录
void Chat_group::on_chatListBtn_clicked()
{
    QString chatLog = displayMsg->toPlainText();
    Chat_log *chat_log = new Chat_log(chatLog);
    chat_log->show();
    chat_log->exec();
}


void Chat_group::on_closeBtn_clicked()
{
    this->close();
}

void Chat_group::returnPressed()
{
    this->on_sendBtn_clicked();
}

// 发送聊天室信息
void Chat_group::on_sendBtn_clicked()
{
    //如果没有初始化在线用户列表的话，先初始化在线用户
    if(this->userList->rowCount() == 0){
        this->on_clicked_onlineUserList();
    }

    if(this->sendMsg->toPlainText().length() == 0){
        QMessageBox::information(this, tr(""), tr("Empty Message cannot be sent!"));
        this->sendMsg->setFocus();
        return;
    }

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    protocol = new ParseProtocol;

    QHostAddress ip = connection->peerAddress();

    int size = sendMsg->toPlainText().count() + 4;

    QString countStr = countStr.number(size);
    protocol->addAMap("Content-Length", countStr);

    protocol->addAMap("Date", time);
    protocol->addContentData(sendMsg->toPlainText());
    QString protocolStr = protocol->sendMESSAGEProtocol(this->username->text());
    connection->sendMsg(protocolStr);

    //清除输入框
    sendMsg->clear();
}


// 关闭窗口要离线
void Chat_group::closeEvent(QCloseEvent *event)
{
    QMessageBox exitBox;
    exitBox.setText(tr("Exit?"));
    exitBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    exitBox.setDefaultButton(QMessageBox::Cancel);
    int ret = exitBox.exec();
    switch(ret){
    case QMessageBox::Ok:
        //发送离线协议
        this->sendLeaveToHost();
        break;
    case QMessageBox::Cancel:
        event->ignore();
        break;
    default:
        break;
    }
}

// 发送离线消息
void Chat_group::sendLeaveToHost()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    protocol = new ParseProtocol;
    protocol->addAMap("Date", time);
    QString protocolStr = protocol->sendLEAVEProtocol(this->username->text());
    connection->sendMsg(protocolStr);
}


// 开启私聊
void Chat_group::getItem(QTableWidgetItem *item)
{
    int clickRow = item->row();
    QTableWidgetItem *usernameItem = this->userList->item(clickRow, 0);
    QString username = usernameItem->text();

    QTableWidgetItem *ipItem = this->userList->item(clickRow, 1);
    QString ip = ipItem->text();

    QTableWidgetItem *portItem = this->userList->item(clickRow, 2);
    QString port = portItem->text();

    QMessageBox::information(this, tr("private chat"), username + " " + ip + " " + port);

    Chat_dialog chat(username, ip, port.toInt());

    chat.show();
    chat.exec();
}


Chat_group::~Chat_group()
{

    delete this->chatListBtn;
    delete this->closeBtn;
    delete this->sendBtn;
}
