#include "chat_dialog.h"
#include "chat_group.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    //Chat_dialog w("xx", "172.18.68.210", 19999);
    //Chat_dialog w("laochx", "192.168.1.108", 5566);
    //w.show();
    Chat_group chat_group;
    chat_group.show();
    //Chat_log log("record");
    //log.show();
    return a.exec();
}
