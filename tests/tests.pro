QT       += core gui network xml testlib concurrent widgets

TARGET = tests
TEMPLATE = app

CONFIG += debug
CONFIG += c++11
CONFIG-=app_bundle

SRC_DIR = $$PWD/../redis-desktop-manager//

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/testcases/*.cpp \
    $$SRC_DIR/dialogs/*.cpp \
    $$SRC_DIR/core/*.cpp \
    $$SRC_DIR/core/ssh/*.cpp \
    $$SRC_DIR/widgets/*.cpp \
    $$SRC_DIR/updater/*.cpp \
    $$SRC_DIR/models/*.cpp \
    $$SRC_DIR/models/items/*.cpp \
    $$SRC_DIR/models/value-view-formatters/*.cpp \

HEADERS  += \
    $$PWD/testcases/*.h \
    $$SRC_DIR/dialogs/*.h \
    $$SRC_DIR/core/*.h \
    $$SRC_DIR/core/ssh/*.h \
    $$SRC_DIR/widgets/*.h \
    $$SRC_DIR/updater/*.h \
    $$SRC_DIR/models/*.h \
    $$SRC_DIR/models/items/*.h \
    $$SRC_DIR/models/value-view-formatters/*.h \

release: DESTDIR = ./../bin/tests
debug:   DESTDIR = ./../bin/tests

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

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
    }
    else { # ubuntu & debian
        LIBS += -Wl,-rpath /usr/local/lib/
        LIBS += /usr/local/lib/libssh2.so

        PRE_TARGETDEPS += /usr/local/lib/libssh2.so

        #code coverage
        QMAKE_CXXFLAGS += --coverage
        QMAKE_LDFLAGS += --coverage
        LIBS += -lgcov
    }
}

INCLUDEPATH += $$PWD/../deps/libssh/include
DEPENDPATH += $$PWD/../deps/libssh/include

INCLUDEPATH += $$PWD/testcases \
    $$SRC_DIR/models \
    $$SRC_DIR/models/items \
    $$SRC_DIR/models/value-view-formatters \
    $$SRC_DIR/core \
    $$SRC_DIR/core/ssh \
    $$SRC_DIR/updater \
    $$SRC_DIR/widgets \
    $$SRC_DIR/dialogs \

FORMS += \
    $$SRC_DIR/forms/*.ui \

RESOURCES += \
    $$SRC_DIR/resources/rdm.qrc

OTHER_FILES += \
    stubs/connections.xml
