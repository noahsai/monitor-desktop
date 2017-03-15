#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T19:37:59
#
#-------------------------------------------------

#---------------------
#假如是其他系统（没有dtk环境的）可以将下面两句删了
#及将main.cpp里面 #include
CONFIG += link_pkgconfig
PKGCONFIG += dtkwidget dtkbase

#---------------------


QT       += core gui
QT += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = painter
TEMPLATE = app


SOURCES += main.cpp\
        monitot.cpp \
    monitorwindow.cpp

HEADERS  += monitot.h \
    monitorwindow.h

FORMS    += monitot.ui \
    monitorwindow.ui

RESOURCES += \
    res.qrc
