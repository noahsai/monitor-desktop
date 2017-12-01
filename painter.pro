#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T19:37:59
#
#-------------------------------------------------



QT       += core gui
QT += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = painter
TEMPLATE = app


SOURCES += main.cpp\
        monitot.cpp \
    monitorwindow.cpp \
    printcpu/manager.cpp \
    printcpu/widget.cpp

HEADERS  += monitot.h \
    monitorwindow.h \
    printcpu/manager.h \
    printcpu/widget.h

FORMS    += monitot.ui \
    monitorwindow.ui \
    printcpu/manager.ui \
    printcpu/widget.ui

RESOURCES += \
    res.qrc

SUBDIRS += \
    printcpu/printcpu.pro

