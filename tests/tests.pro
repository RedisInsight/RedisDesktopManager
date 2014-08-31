QT       += core gui network xml testlib concurrent widgets

TARGET = tests
TEMPLATE = app

CONFIG += debug c++11
CONFIG-=app_bundle

SOURCES += \
    $$PWD/main.cpp \    

SRC_DIR = $$PWD/../src//

RESOURCES += \
    $$SRC_DIR/resources/rdm.qrc

INCLUDEPATH += $$SRC_DIR/modules/

#DEFINES += INTEGRATION_TESTS

#TEST CASES
#include($$PWD/testcases/app/app-tests.pri)
include($$PWD/testcases/connections-tree/connections-tree-tests.pri)
include($$PWD/testcases/redisclient/redisclient-tests.pri)
include($$PWD/testcases/console/console-tests.pri)
include($$PWD/testcases/value-editor/value-editor-tests.pri)
#############

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

