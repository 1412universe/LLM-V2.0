#-------------------------------------------------
#
# Project created by QtCreator 2025-02-18T18:03:53
#
#-------------------------------------------------

QT       += core gui charts axcontainer multimedia
QT       += network
QT       += texttospeech
QT        += axcontainer


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatDemo_QtUI
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
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    icons/ai.png \
    icons/user.png \
    icons/ai.png \
    icons/user.png

# Poppler 库路径
#INCLUDEPATH += $$PWD/poppler
#win32: LIBS += -L$$PWD/poppler -llibpoppler
#win32: LIBS += -L$$PWD/poppler -llibpoppler-qt5
