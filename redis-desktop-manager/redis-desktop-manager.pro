#-------------------------------------------------
#
# Redis Desktop Manager
#
#-------------------------------------------------

QT       += core gui network xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rdm
TEMPLATE = app

CONFIG -= debug
CONFIG += c++11 release
#CONFIG-=app_bundle

SOURCES += \
    $$PWD/source/main.cpp \
    $$PWD/source/demo.cpp \
    $$PWD/source/connection.cpp \
    $$PWD/source/widgets/*.cpp \
    $$PWD/source/updater/Updater.cpp \
    $$PWD/source/redis/*.cpp \
    $$PWD/source/network/*.cpp \
    $$PWD/source/models/*.cpp \
    $$PWD/source/models/items/*.cpp \
    $$PWD/source/models/value-view-formatters/*.cpp \

HEADERS  += \
    $$PWD/include/*.h \
    $$PWD/include/widgets/*.h \
    $$PWD/include/updater/Updater.h \
    $$PWD/include/redis/*.h \
    $$PWD/include/network/*.h \
    $$PWD/include/models/*.h \
    $$PWD/include/models/items/*.h \
    $$PWD/include/models/value-view-formatters/*.h \


release: DESTDIR = ./../bin/linux/release
debug:   DESTDIR = ./../bin/linux/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32 {

    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../deps/libs/win32/ -llibssh2
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/libssh2.lib

        LIBS += -L$$PWD/../deps/libs/win32/ -ljsoncpp
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/jsoncpp.lib
    }

    else: CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../deps/libs/win32/ -llibssh2
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/libssh2.lib

        LIBS += -L$$PWD/../deps/libs/win32/ -ljsoncppd
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/jsoncppd.lib
    }
}

unix {
    macx { # os x 10.8
        LIBS += /usr/local/lib/libssh2.dylib /usr/local/lib/libjsoncpp.a
        PRE_TARGETDEPS += /usr/local/lib/libssh2.dylib /usr/local/lib/libjsoncpp.a

        QMAKE_INFO_PLIST = Info.plist
        ICON = rdm.icns
    }
    else { # ubuntu & debian
        LIBS += -Wl,-rpath /usr/local/lib/
        LIBS += /usr/local/lib/libssh2.so /usr/local/lib/libjsoncpp.a

        PRE_TARGETDEPS += /usr/local/lib/libssh2.so        
        PRE_TARGETDEPS += /usr/local/lib/libjsoncpp.a

        target.path = /usr/share/redis-desktop-manager/bin
        target.files = $$DESTDIR/rdm qt.conf rdm.png
        INSTALLS += target

        deskicon.path = /usr/share/applications
        deskicon.files = rdm.desktop
        INSTALLS += deskicon

        data.path = /usr/share/redis-desktop-manager/lib
        data.files = lib/*
        INSTALLS += data
    }
}

INCLUDEPATH += $$PWD/../deps/libssh/include
DEPENDPATH += $$PWD/../deps/libssh/include

INCLUDEPATH += $$PWD/../deps/jsoncpp/include
DEPENDPATH += $$PWD/../deps/jsoncpp/include
INCLUDEPATH += $$PWD/../deps/jsoncpp/include
DEPENDPATH += $$PWD/../deps/jsoncpp/include

INCLUDEPATH += $$PWD/source \
    $$PWD/source/models \
    $$PWD/source/models/items \
    $$PWD/source/network \
    $$PWD/source/redis \
    $$PWD/source/updater \
    $$PWD/source/widgets \
    $$PWD/"include" \
    $$PWD/include/models \
    $$PWD/include/models/items \
    $$PWD/include/models/value-view-formatters \
    $$PWD/include/network \
    $$PWD/include/redis \
    $$PWD/include/updater \
    $$PWD/include/widgets \

FORMS += \
    $$PWD/forms/*.ui \

RESOURCES += \
    Resources/demo.qrc

OTHER_FILES += \
    qt.conf \
    Info.plist


