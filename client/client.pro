#-------------------------------------------------
#
# Project created by QtCreator 2014-07-18T21:46:37
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = client
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    coreclient.cpp \
    rawclient.cpp

HEADERS += \
    coreclient.h \
    rawclient.h
