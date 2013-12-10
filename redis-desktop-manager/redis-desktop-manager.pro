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
CONFIG += release
QMAKE_CXXFLAGS += -std=gnu++0x #workaround for google breakpad

#CONFIG-=app_bundle

SOURCES += \
    $$PWD/source/main.cpp \
    $$PWD/source/demo.cpp \
    $$PWD/source/connection.cpp \
    $$PWD/source/widgets/*.cpp \
    $$PWD/source/crashhandler/*.cpp \
    $$PWD/source/updater/Updater.cpp \
    $$PWD/source/redis/*.cpp \
    $$PWD/source/network/*.cpp \
    $$PWD/source/models/*.cpp \
    $$PWD/source/models/items/*.cpp \
    $$PWD/source/models/value-view-formatters/*.cpp \

HEADERS  += \
    $$PWD/include/*.h \
    $$PWD/include/widgets/*.h \
    $$PWD/include/crashhandler/*.h \
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

BREAKPADDIR = $$PWD/../deps/google-breakpad/src

win32 {

    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../deps/libs/win32/release/ -llibssh2
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/release/libssh2.lib
    }

    else: CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../deps/libs/win32/debug/ -llibssh2
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/debug/libssh2.lib
    }
}

unix {
    macx { # os x 10.8
        LIBS += /usr/local/lib/libssh2.dylib
        PRE_TARGETDEPS += /usr/local/lib/libssh2.dylib

        INCLUDEPATH += $$PWD/../deps/libssh/include

        QMAKE_INFO_PLIST = Info.plist
        ICON = rdm.icns
    }
    else { # ubuntu & debian
        LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib #don't remove!!!
        LIBS += /usr/local/lib/libssh2.so  /usr/local/lib/libbreakpad.a /usr/local/lib/libbreakpad_client.a

        PRE_TARGETDEPS +=/usr/local/lib/libssh2.so \
                         /usr/local/lib/libbreakpad.a \
                         /usr/local/lib/libbreakpad_client.a \

        INCLUDEPATH += $$BREAKPADDIR/ \
            $$BREAKPADDIR/client/linux/handler/ \
            $$BREAKPADDIR/client/linux/crash_generation/ \
            $$BREAKPADDIR/client/linux/minidump_writer/ \
            $$BREAKPADDIR/client/linux/log/ \
            $$BREAKPADDIR/client/ \
            $$BREAKPADDIR/common/linux/ \
            $$BREAKPADDIR/common/ \
            $$BREAKPADDIR/google_breakpad/common/ \
            $$BREAKPADDIR/processor/ \
            $$BREAKPADDIR/third_party/lss/ \

        DEPENDPATH += $$BREAKPADDIR/ \
            $$BREAKPADDIR/client/linux/handler/ \
            $$BREAKPADDIR/client/linux/crash_generation/ \
            $$BREAKPADDIR/client/linux/minidump_writer/ \
            $$BREAKPADDIR/client/linux/log/ \
            $$BREAKPADDIR/client/ \
            $$BREAKPADDIR/common/linux/ \
            $$BREAKPADDIR/common/ \
            $$BREAKPADDIR/google_breakpad/common/ \
            $$BREAKPADDIR/processor/ \
            $$BREAKPADDIR/third_party/lss/ \

        target.path = /usr/share/redis-desktop-manager/bin
        target.files = $$DESTDIR/rdm $$DESTDIR/crashreporter qt.conf rdm.png
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
DEPENDPATH += $$BREAKPADDIR

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
    $$PWD/include/crashhandler \
    $$PWD/include/widgets \

FORMS += \
    $$PWD/forms/*.ui \

RESOURCES += \
    Resources/demo.qrc

OTHER_FILES += \
    qt.conf \
    Info.plist


