#-------------------------------------------------
#
# Redis Desktop Manager
#
#-------------------------------------------------

QT += core gui network xml concurrent widgets

TARGET = rdm
TEMPLATE = app

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/dialogs/*.cpp \
    $$PWD/core/*.cpp \
    $$PWD/core/ssh/*.cpp \
    $$PWD/widgets/*.cpp \
    $$PWD/crashhandler/*.cpp \
    $$PWD/updater/*.cpp \
    $$PWD/utils/*.cpp \
    $$PWD/models/*.cpp \
    $$PWD/models/items/*.cpp \
    $$PWD/models/value-view-formatters/*.cpp \

HEADERS  += \
    $$PWD/version.h \
    $$PWD/dialogs/*.h \
    $$PWD/core/*.h \
    $$PWD/core/ssh/*.h \
    $$PWD/widgets/*.h \
    $$PWD/crashhandler/*.h \
    $$PWD/updater/*.h \
    $$PWD/utils/*.h \
    $$PWD/models/*.h \
    $$PWD/models/items/*.h \
    $$PWD/models/value-view-formatters/*.h \
    $$PWD/core/core.h

FORMS += \
    $$PWD/forms/*.ui \

THIRDPARTYDIR = $$PWD/../3rdparty/

include($$THIRDPARTYDIR/3rdparty.pri)

win32-msvc* {
    CONFIG += c++11
    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
    RC_FILE += $$PWD/resources/rdm.rc

    release: DESTDIR = ./../bin/windows/release
    debug:   DESTDIR = ./../bin/windows/debug        
}

unix:macx { # OSX
    CONFIG += c++11 release
    CONFIG -= debug

    debug {
        CONFIG-=app_bundle
    }

    release: DESTDIR = ./../bin/linux/release
    debug:   DESTDIR = ./../bin/linux/debug

    #deployment
    QMAKE_INFO_PLIST =  $$PWD/resources/Info.plist
    ICON = $$PWD/resources/rdm.icns

    CRASHREPORTER_APP.files = $$DESTDIR/crashreporter
    CRASHREPORTER_APP.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += CRASHREPORTER_APP
}

unix:!macx { # ubuntu & debian

    CONFIG += static release
    CONFIG -= debug

    #Qt static linked on Ubuntu/Debian
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++

    release: DESTDIR = ./../bin/linux/release
    debug:   DESTDIR = ./../bin/linux/debug


    #deployment
    target.path = /usr/share/redis-desktop-manager/bin
    target.files = $$DESTDIR/rdm $$DESTDIR/crashreporter  $$PWD/resources/qt.conf  $$PWD/resources/rdm.png
    INSTALLS += target

    data.path = /usr/share/redis-desktop-manager/lib
    data.files = lib/*
    INSTALLS += data

    deskicon.path = /usr/share/applications
    deskicon.files =  $$PWD/resources/rdm.desktop
    INSTALLS += deskicon
}

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/obj

INCLUDEPATH += $$PWD/dialogs \
    $$PWD/core/ \
    $$PWD/core/ssh/ \
    $$PWD/widgets/ \
    $$PWD/crashhandler/ \
    $$PWD/updater/ \
    $$PWD/models/ \
    $$PWD/models/items/ \
    $$PWD/models/value-view-formatters/ \
    $$PWD/resourses \
    $$PWD/ \

RESOURCES += \
    $$PWD/resources/rdm.qrc \
    $$PWD/resources/fonts.qrc

OTHER_FILES += \
    qt.conf \
    Info.plist


