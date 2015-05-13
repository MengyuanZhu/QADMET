#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T16:20:35
#
#-------------------------------------------------

QT       += core gui
QT += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QADMET
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filter.cpp

HEADERS  += mainwindow.h \
    filter.h \
    svm.h

FORMS    += mainwindow.ui \
    filter.ui

INCLUDEPATH+=/usr/include/openbabel-2.0/


LIBS += -lopenbabel
