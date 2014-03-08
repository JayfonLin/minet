#ifndef CHAT_LOG_H
#define CHAT_LOG_H
#include <QtWidgets>
#include <QtGui>
#include <QToolButton>
#include <QTextBrowser>
class Chat_log : public QDialog
{
    Q_OBJECT

public:
    Chat_log(QString);
    ~Chat_log();

private:
    QTextBrowser *chatLog;				//聊天记录
    QPushButton *saveBtn;				//保存按钮
    QString chatRecord;
    void initUi();						//初始化ui界面

private slots:
    void on_saveBtn_clicked();          //保存聊天记录

};

#endif
