#-------------------------------------------------
#
# Project created by QtCreator 2014-07-17T21:26:44
#
#-------------------------------------------------

QT       += core network
QT       -= gui
QT       += websockets

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    coreserver.cpp \
    rawserver.cpp \
    ../common/common.cpp \
    wsoutput.cpp

HEADERS += \
    coreserver.h \
    rawserver.h \
    ../common/common.h \
    wsoutput.h
