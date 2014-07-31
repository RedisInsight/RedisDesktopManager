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
    $$SRC_DIR/connections-tree/model.cpp \
    $$SRC_DIR/console/consoletab.cpp \
    $$SRC_DIR/core/*.cpp \
    $$SRC_DIR/core/protocols/*.cpp \
    $$SRC_DIR/core/ssh/*.cpp \
    $$SRC_DIR/core/transporters/*.cpp \
    $$SRC_DIR/models/connectionsmanager.cpp \

HEADERS  += \
    $$PWD/testcases/*.h \
    $$PWD/mocks/*.h \
    $$SRC_DIR/connections-tree/items/*.h \
    $$SRC_DIR/connections-tree/iconproxy.h \
    $$SRC_DIR/connections-tree/operations.h \
    $$SRC_DIR/connections-tree/model.h \
    $$SRC_DIR/console/consoletab.h \
    $$SRC_DIR/console/operations.h \
    $$SRC_DIR/widgets/basetab.h \
    $$SRC_DIR/core/*.h \
    $$SRC_DIR/core/protocols/*.h \
    $$SRC_DIR/core/ssh/*.h \
    $$SRC_DIR/core/transporters/*.h \
    $$SRC_DIR/models/connectionsmanager.h \


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

FORMS += \
    $$SRC_DIR/forms/*.ui \

RESOURCES += \
    $$SRC_DIR/resources/rdm.qrc

OTHER_FILES += \
    stubs/connections.xml
