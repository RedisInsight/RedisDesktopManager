QT       += core gui network concurrent widgets quick quickwidgets testlib

TARGET = tests
TEMPLATE = app

CONFIG += debug c++11
CONFIG-=app_bundle

HEADERS += \
    $$PWD/basetestcase.h \

SOURCES += \
    $$PWD/main.cpp \    

PROJECT_ROOT = $$PWD/../..//
SRC_DIR = $$PROJECT_ROOT/src//

RESOURCES += \
    $$SRC_DIR/resources/rdm.qrc

INCLUDEPATH += $$SRC_DIR/modules/ \
    $$SRC_DIR/ \
    $$PWD/

DEFINES += INTEGRATION_TESTS
DEFINES += ELPP_STL_LOGGING ELPP_DISABLE_DEFAULT_CRASH_HANDLING

#TEST CASES
include($$PWD/testcases/app/app-tests.pri)
include($$PWD/testcases/connections-tree/connections-tree-tests.pri)
include($$PWD/testcases/redisclient/redisclient-tests.pri)
include($$PWD/testcases/console/console-tests.pri)
include($$PWD/testcases/value-editor/value-editor-tests.pri)
#############

release: DESTDIR = $$PROJECT_ROOT/bin/tests
debug:   DESTDIR = $$PROJECT_ROOT/bin/tests

win32-msvc* {
    CONFIG += c++11
    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32

    release: DESTDIR = .$$PROJECT_ROOT/bin/windows/release
    debug:   DESTDIR = $$PROJECT_ROOT/bin/windows/debug
}

win32 {
    CONFIG -= debug
    CONFIG += release
    include($$PROJECT_ROOT/3rdparty/3rdparty.pri)
}
unix:!mac {
    LIBS += -lssh2 -lssl -lcrypto -ldl -lz
    INCLUDEPATH += $$PROJECT_ROOT/3rdparty/qtconsole/include
    
    #3rdparty
    HEADERS += $$PROJECT_ROOT/3rdparty/qtconsole/include/qconsole.h
    SOURCES += $$PROJECT_ROOT/3rdparty/qtconsole/src/qconsole.cpp
    INCLUDEPATH += $$PROJECT_ROOT/3rdparty/easyloggingpp/src
    HEADERS += $$PROJECT_ROOT/3rdparty/easyloggingpp/src/easylogging++.h

    #code coverage
    QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0
    QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
    LIBS += -lgcov
}

unix:mac {
    LIBS += /usr/local/lib/libssh2.dylib
    LIBS += -lz
    INCLUDEPATH += $$PROJECT_ROOT/3rdparty/qtconsole/include

    #3rdparty
    HEADERS += $$PROJECT_ROOT/3rdparty/qtconsole/include/qconsole.h
    SOURCES += $$PROJECT_ROOT/3rdparty/qtconsole/src/qconsole.cpp
    INCLUDEPATH += $$PROJECT_ROOT/3rdparty/easyloggingpp/src
    HEADERS += $$PROJECT_ROOT/3rdparty/easyloggingpp/src/easylogging++.h
}

INCLUDEPATH += $$PROJECT_ROOT/3rdparty/libssh2/include
DEPENDPATH += $$PROJECT_ROOT/3rdparty/libssh2/include

UI_DIR = $$DESTDIR/ui
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/obj
