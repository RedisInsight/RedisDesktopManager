#-------------------------------------------------
#
# Crash Reporter
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = crashreporter
TEMPLATE = app

CONFIG -= debug
CONFIG += release c++11

CONFIG-=app_bundle

SOURCES += \
    $$PWD/*.cpp \

HEADERS  += \
    $$PWD/*.h \

FORMS += \
    $$PWD/*.ui \

windows {
    release: DESTDIR = ./../bin/windows/release
    debug:   DESTDIR = ./../bin/windows/debug
} else {
    release: DESTDIR = ./../bin/linux/release
    debug:   DESTDIR = ./../bin/linux/debug
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32 {
    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
}

unix:!macx {
    LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib #don't remove!!!
}

macx {
    CONFIG-=app_bundle
}

INCLUDEPATH += $$PWD/../redis-desktop-manager
DEPENDPATH += $$PWD/../redis-desktop-manager
