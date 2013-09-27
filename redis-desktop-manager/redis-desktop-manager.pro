#-------------------------------------------------
#
# Redis Desktop Manager
#
#-------------------------------------------------

QT       += core gui network xml dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rdm
TEMPLATE = app

CONFIG -= debug
CONFIG += c++11 release

SOURCES += \
    source/main.cpp \
    source/demo.cpp \
    source/connection.cpp \
    source/widgets/zsetViewTab.cpp \
    source/widgets/stringViewTab.cpp \
    source/widgets/listViewTab.cpp \
    source/widgets/hashViewTab.cpp \
    source/updater/Updater.cpp \
    source/redis/RedisConnectionsManager.cpp \
    source/redis/RedisConnectionOverSsh.cpp \
    source/redis/RedisConnectionConfig.cpp \
    source/redis/RedisConnectionAbstract.cpp \
    source/redis/RedisConnection.cpp \
    source/network/qxtsshtcpsocket.cpp \
    source/network/qxtsshprocess.cpp \
    source/network/qxtsshclient.cpp \
    source/network/qxtsshchannel.cpp \
    source/models/SortedSetKeyModel.cpp \
    source/models/RedisConnectionsModel.cpp \
    source/models/ListKeyModel.cpp \
    source/models/HashKeyModel.cpp \
    source/models/items/RedisServerItem.cpp \
    source/models/items/RedisServerDbItem.cpp \
    source/models/items/RedisKeyNamespace.cpp \
    source/models/items/RedisKeyItem.cpp

HEADERS  += \
    include/demo.h \
    include/connection.h \
    include/widgets/zsetViewTab.h \
    include/widgets/stringViewTab.h \
    include/widgets/listViewTab.h \
    include/widgets/hashViewTab.h \
    include/updater/Updater.h \
    include/redis/RedisException.h \
    include/redis/RedisConnectionsManager.h \
    include/redis/RedisConnectionOverSsh.h \
    include/redis/RedisConnectionConfig.h \
    include/redis/RedisConnectionAbstract.h \
    include/redis/RedisConnection.h \
    include/network/qxtsshtcpsocket.h \
    include/network/qxtsshprocess.h \
    include/network/qxtsshclient_p.h \
    include/network/qxtsshclient.h \
    include/network/qxtsshchannel_p.h \
    include/network/qxtsshchannel.h \
    include/network/qxtnetwork.h \
    include/models/SortedSetKeyModel.h \
    include/models/RedisConnectionsModel.h \
    include/models/ListKeyModel.h \
    include/models/HashKeyModel.h \
    include/models/items/RedisServerItem.h \
    include/models/items/RedisServerDbItem.h \
    include/models/items/RedisKeyNamespace.h \
    include/models/items/RedisKeyItem.h

release: DESTDIR = ./../bin/linux/release
debug:   DESTDIR = ./../bin/linux/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../deps/libs/win32/debug/ -llibssh2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../deps/libs/win32/debug/ -llibssh2
else:unix: LIBS += /usr/local/lib/libssh2.so

win32:CONFIG(release, debug|release): LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
else:win32:CONFIG(debug, debug|release): LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32


INCLUDEPATH += $$PWD/../deps/libssh/include
DEPENDPATH += $$PWD/../deps/libssh/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../deps/libs/win32/debug/libssh2.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../deps/libs/win32/debug/libssh2.lib
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


