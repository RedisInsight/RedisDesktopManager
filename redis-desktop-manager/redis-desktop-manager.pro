#-------------------------------------------------
#
# Redis Desktop Manager
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rdm
TEMPLATE = app

CONFIG -= debug
CONFIG += c++11 release

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

HEADERS  += \
    $$PWD/include/*.h \
    $$PWD/include/widgets/*.h \
    $$PWD/include/updater/Updater.h \
    $$PWD/include/redis/*.h \
    $$PWD/include/network/*.h \
    $$PWD/include/models/*.h \
    $$PWD/include/models/items/*.h \

release: DESTDIR = ./../bin/linux/release
debug:   DESTDIR = ./../bin/linux/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../deps/libs/win32/ -llibssh2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../deps/libs/win32/ -llibssh2
else:unix: LIBS += /usr/local/lib/libssh2.so

win32:CONFIG(release, debug|release): LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
else:win32:CONFIG(debug, debug|release): LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32


INCLUDEPATH += $$PWD/../deps/libssh/include
DEPENDPATH += $$PWD/../deps/libssh/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../deps/libs/win32/libssh2.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../deps/libs/win32/libssh2.lib
else:unix: PRE_TARGETDEPS += /usr/local/lib/libssh2.so

unix:!mac {
 LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib
}

target.path = /usr/share/redis-desktop-manager/bin
target.files = $$DESTDIR/rdm qt.conf rdm.png
INSTALLS += target

deskicon.path = /usr/share/applications
deskicon.files = rdm.desktop
INSTALLS += deskicon

data.path = /usr/share/redis-desktop-manager/lib
data.files = lib/*
INSTALLS += data

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
    $$PWD/include/network \
    $$PWD/include/redis \
    $$PWD/include/updater \
    $$PWD/include/widgets \

FORMS += \
    forms/zsetViewTab.ui \
    forms/stringViewTab.ui \
    forms/listViewTab.ui \
    forms/hashViewTab.ui \
    forms/demo.ui \
    forms/connection.ui

RESOURCES += \
    Resources/demo.qrc


OTHER_FILES += \
    qt.conf


