#-------------------------------------------------
#
# Project created by QtCreator 2019-01-07T23:48:41
#
#-------------------------------------------------

QT       += core gui x11extras thelib
SHARE_APP_NAME = theheartbeat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = theheartbeat
TEMPLATE = app

unix:!macx {
    # Include the-libs build tools
    include(/usr/share/the-libs/pri/buildmaster.pri)

    CONFIG += link_pkgconfig
    PKGCONFIG += x11

    target.path = /usr/bin

    desktop.path = /usr/share/applications
    desktop.files = com.vicr123.theheartbeat.desktop

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = theheartbeat.svg

    INSTALLS += target desktop icon
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    processes/process.cpp \
    processes/processmanager.cpp \
    processes/processmodel.cpp \
    system/systemmanager.cpp \
    panes/percentagepane.cpp \
    panes/minmaxpane.cpp \
    processaction.cpp \
    panes/minipercentagepane.cpp \
    panes/sidepane.cpp \
    panes/numberpane.cpp \
    panes/mininumberpane.cpp

HEADERS += \
        mainwindow.h \
    processes/process.h \
    processes/processmanager.h \
    processes/processmodel.h \
    system/systemmanager.h \
    panes/percentagepane.h \
    panes/minmaxpane.h \
    processaction.h \
    panes/minipercentagepane.h \
    panes/sidepane.h \
    panes/numberpane.h \
    panes/mininumberpane.h

FORMS += \
        mainwindow.ui \
    panes/percentagepane.ui \
    panes/minmaxpane.ui \
    processaction.ui \
    panes/minipercentagepane.ui \
    panes/numberpane.ui \
    panes/mininumberpane.ui

# Turn off stripping as this causes the install to fail :(
QMAKE_STRIP = echo

DISTFILES += \
    com.vicr123.theheartbeat.desktop

RESOURCES += \
    resources.qrc
