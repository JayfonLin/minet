#include "chat_log.h"

Chat_log::Chat_log(QString chatRecord)
{
    this->chatRecord = chatRecord;
    this->initUi();
}

void Chat_log::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    //初始化聊天记录框
    chatLog = new QTextBrowser;
    chatLog->setMinimumSize(450, 500);
    chatLog->setText(chatRecord);



    //saveBtn
    saveBtn = new QPushButton;
    saveBtn->setText(tr("save"));
    saveBtn->setMaximumWidth(100);
    mainLayout->addWidget(chatLog);
    mainLayout->addWidget(saveBtn, 0, Qt::AlignLeft);
    connect(saveBtn, SIGNAL(clicked()), this, SLOT(on_saveBtn_clicked()));
    setLayout(mainLayout);
}

void Chat_log::on_saveBtn_clicked()
{
    //QFile
    QFile file("log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << chatRecord;


    QMessageBox tmp;
    tmp.setText(tr("Save completed. Stored in ..\\log.txt"));
    tmp.exec();
}

Chat_log::~Chat_log()
{
    delete this->saveBtn;
}
