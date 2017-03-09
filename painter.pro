#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T19:37:59
#
#-------------------------------------------------

QT       += core gui

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
