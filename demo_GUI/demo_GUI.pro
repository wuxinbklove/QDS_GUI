#-------------------------------------------------
#
# Project created by QtCreator 2018-05-11T22:34:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo_GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    loginwindow.cpp \
    newslistener.cpp \
    mdisubwindow.cpp \
    qselectcombo.cpp

HEADERS += \
        mainwindow.h \
    loginwindow.h \
    newslistener.h \
    mdisubwindow.h \
    qselectcombo.h

FORMS += \
        mainwindow.ui \
    loginwindow.ui \
    mdisubview.ui

unix|win32: LIBS += -L$$PWD/SDK/lib/ -lwQDSApi

INCLUDEPATH += $$PWD/SDK/include
DEPENDPATH += $$PWD/SDK/include

RESOURCES += \
    resources.qrc
