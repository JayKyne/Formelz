#-------------------------------------------------
#
# Project created by QtCreator 2019-05-31T17:02:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD/klfbackend \
               "framelesswindow"

TARGET = Formelz
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11

SOURCES += \
        data.cpp \
        functions.cpp \
        main.cpp \
        framelesswindow/framelesswindow.cpp \
        framelesswindow/windowdragger.cpp \
        DarkStyle.cpp \
        mainwindow.cpp


HEADERS += data.h \
           functions.h \
           framelesswindow/framelesswindow.h \
           framelesswindow/windowdragger.h \
           DarkStyle.h \
           mainwindow.h

FORMS += \
         framelesswindow/framelesswindow.ui \
         mainwindow.ui

RESOURCES += darkstyle.qrc \
             framelesswindow.qrc

# note that in unix(linux) systems library names are case sensitive
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/build-KLFBackend/ -lklfbackend
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/build-KLFBackend/ -lklfbackend
else:unix: LIBS += -L$$PWD/build-KLFBackend/ -lKLFBackend

RC_ICONS = logo.ico
