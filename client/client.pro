#-------------------------------------------------
#
# Project created by QtCreator 2014-07-18T21:46:37
#
#-------------------------------------------------

QT       += gui network widgets

TARGET = client
CONFIG   -= app_bundle

TEMPLATE = app

#INCLUDEPATH += ../common

SOURCES += main.cpp \
    coreclient.cpp \
    rawclient.cpp \
    ../common/common.cpp \
    clientmainwindow.cpp

HEADERS += \
    coreclient.h \
    rawclient.h \
    ../common/common.h \
    clientmainwindow.h
