QT       += core gui network xml concurrent widgets quick quickwidgets testlib

TARGET = tests
TEMPLATE = app

CONFIG += debug c++11
CONFIG-=app_bundle

HEADERS += \
    $$PWD/basetestcase.h \

SOURCES += \
    $$PWD/main.cpp \    

SRC_DIR = $$PWD/../src//

RESOURCES += \
    $$SRC_DIR/resources/rdm.qrc

INCLUDEPATH += $$SRC_DIR/modules/ \
    $$SRC_DIR/ \
    $$PWD/

#DEFINES += INTEGRATION_TESTS

#TEST CASES
include($$PWD/testcases/app/app-tests.pri)
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

win32 {
    include($$PWD/../3rdparty/3rdparty.pri)
}
unix:!mac {
    LIBS += -lssh2 -lssl -lcrypto -ldl -lz
    INCLUDEPATH += $$PWD/../3rdparty/qtconsole/include
    
    #3rdparty
    HEADERS += $$PWD/../3rdparty/qtconsole/include/qconsole.h
    SOURCES += $$PWD/../3rdparty/qtconsole/src/qconsole.cpp
    INCLUDEPATH += $$PWD/../3rdparty/easyloggingpp/src
    HEADERS += $$PWD/../3rdparty/easyloggingpp/src/easylogging++.h

    #code coverage
    QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0
    QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
    LIBS += -lgcov
}

INCLUDEPATH += $$PWD/../3rdparty/libssh2/include
DEPENDPATH += $$PWD/../3rdparty/libssh2/include

UI_DIR = $$DESTDIR/ui
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/obj
