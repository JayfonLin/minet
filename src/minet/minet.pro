#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T13:18:22
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = minet
TEMPLATE = app


SOURCES += main.cpp\
    connection.cpp \
    chat_dialog.cpp \
    chat_group.cpp \
    chat_log.cpp \
    p2pmessage.cpp \
    parseprotocol.cpp \
   #mainwindow.cpp

HEADERS  += macro.h \ #mainwindow.h \
    connection.h \
    chat_dialog.h \
    chat_group.h \
    chat_log.h \
    p2pmessage.h \
    parseprotocol.h \


FORMS    +=
