#-------------------------------------------------
#
# Project created by QtCreator 2014-07-17T21:26:44
#
#-------------------------------------------------

QT       += core network

QT       -= gui

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

INCLUDEPATH += ./QtWebsocket
DEPENDPATH += ./QtWebsocket

win32:CONFIG(release, debug|release): LIBS += -LQtWebsocket/release/ -lQtWebsocket
else:win32:CONFIG(debug, debug|release): LIBS += -LQtWebsocket/debug/ -lQtWebsocket
else:unix: LIBS += -L./QtWebsocket/ -lQtWebsocket

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += QtWebsocket/release/libQtWebsocket.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += QtWebsocket/debug/libQtWebsocket.a
else:unix: PRE_TARGETDEPS += ./QtWebsocket/libQtWebsocket.a
