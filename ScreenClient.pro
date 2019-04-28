#-------------------------------------------------
#
# Project created by QtCreator 2019-04-08T15:44:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScreenClient
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



#######################linux##################################
unix{
INCLUDEPATH += -I ./ffmpeg/include

LIBS += -L$$PWD/ffmpeg/lib/arm -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale

}

#######################window##################################
win32{
INCLUDEPATH += -I ./ffmpeg/include

LIBS += $$PWD/ffmpeg/lib/win/libavcodec.dll.a \
        $$PWD/ffmpeg/lib/win/libavdevice.dll.a \
        $$PWD/ffmpeg/lib/win/libavfilter.dll.a \
        $$PWD/ffmpeg/lib/win/libavformat.dll.a \
        $$PWD/ffmpeg/lib/win/libavutil.dll.a \
        $$PWD/ffmpeg/lib/win/libpostproc.dll.a \
        $$PWD/ffmpeg/lib/win/libswresample.dll.a \
        $$PWD/ffmpeg/lib/win/libswscale.dll.a -lws2_32
}
################################################################



CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    recvthread.cpp \
    decodethread.cpp \
    queue.cpp

HEADERS += \
        mainwindow.h \
    recvthread.h \
    decodethread.h \
    queue.h

FORMS += \
        mainwindow.ui



