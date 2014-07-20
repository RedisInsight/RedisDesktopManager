QT       += core gui network xml testlib concurrent widgets

TARGET = tests
TEMPLATE = app

CONFIG += debug
CONFIG += c++11
CONFIG-=app_bundle

SRC_DIR = $$PWD/../src//

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/testcases/*.cpp \    
    $$SRC_DIR/connections-tree/items/*.cpp \
    $$SRC_DIR/connections-tree/iconproxy.cpp \

HEADERS  += \
    $$PWD/testcases/*.h \
    $$PWD/mocks/*.h \
    $$SRC_DIR/connections-tree/items/*.h \
    $$SRC_DIR/connections-tree/iconproxy.h \
    $$SRC_DIR/connections-tree/operations.h \


release: DESTDIR = ./../bin/tests
debug:   DESTDIR = ./../bin/tests

win32-msvc* {
    CONFIG += c++11
    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32

    release: DESTDIR = ./../bin/windows/release
    debug:   DESTDIR = ./../bin/windows/debug
}

THIRDPARTYDIR = $$PWD/../3rdparty/

include($$THIRDPARTYDIR/3rdparty.pri)

INCLUDEPATH += $$PWD/../3rdparty/libssh2/include
DEPENDPATH += $$PWD/../3rdparty/libssh2/include

INCLUDEPATH += $$PWD/testcases \
    $$PWD/mocks \
    $$SRC_DIR \
    $$SRC_DIR/connections-tree \

FORMS += \
    $$SRC_DIR/forms/*.ui \

RESOURCES += \
    $$SRC_DIR/resources/rdm.qrc

OTHER_FILES += \
    stubs/connections.xml
