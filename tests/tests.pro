QT       += core gui network xml testlib concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tests
TEMPLATE = app

CONFIG -= debug
CONFIG += c++11 release
CONFIG-=app_bundle

SRC_DIR = $$PWD/../redis-desktop-manager//

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/source/*.cpp \
    $$SRC_DIR/source/updater/Updater.cpp \
    $$SRC_DIR/source/redis/*.cpp \
    $$SRC_DIR/source/network/*.cpp \
    $$SRC_DIR/source/models/*.cpp \
    $$SRC_DIR/source/models/items/*.cpp \
    $$SRC_DIR/source/models/value-view-formatters/*.cpp \
    $$SRC_DIR/source/widgets/consoleTab.cpp \

HEADERS  += \
    $$PWD/include/*.h \
    $$SRC_DIR/include/updater/Updater.h \
    $$SRC_DIR/include/redis/*.h \
    $$SRC_DIR/include/network/*.h \
    $$SRC_DIR/include/models/*.h \
    $$SRC_DIR/include/models/items/*.h \
    $$SRC_DIR/include/models/value-view-formatters/*.h \
    $$SRC_DIR/include/widgets/consoleTab.h \

release: DESTDIR = ./../bin/tests
debug:   DESTDIR = ./../bin/tests

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32 {

    LIBS += -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../deps/libs/win32/ -llibssh2
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/libssh2.lib

        LIBS += -L$$PWD/../deps/libs/win32/ -ljsoncpp
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/jsoncpp.lib
    }

    else: CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../deps/libs/win32/ -llibssh2
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/libssh2.lib

        LIBS += -L$$PWD/../deps/libs/win32/ -ljsoncppd
        PRE_TARGETDEPS += $$PWD/../deps/libs/win32/jsoncppd.lib
    }
}

unix {
    macx { # os x 10.8
        LIBS += /usr/local/lib/libssh2.dylib
        PRE_TARGETDEPS += /usr/local/lib/libssh2.dylib
    }
    else { # ubuntu & debian
        LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib
        LIBS += /usr/local/lib/libssh2.so
        PRE_TARGETDEPS += /usr/local/lib/libssh2.so
    }
}

INCLUDEPATH += $$PWD/../deps/libssh/include
DEPENDPATH += $$PWD/../deps/libssh/include

INCLUDEPATH += $$PWD/../deps/json-cpp/include
DEPENDPATH += $$PWD/../deps/json-cpp/include

INCLUDEPATH += $$PWD/source \
    $$PWD/"include" \
    $$SRC_DIR/source/models \
    $$SRC_DIR/source/models/items \
    $$SRC_DIR/source/network \
    $$SRC_DIR/source/redis \
    $$SRC_DIR/source/updater \
    $$SRC_DIR/source/widgets \
    $$SRC_DIR/"include" \
    $$SRC_DIR/include/models \
    $$SRC_DIR/include/models/items \
    $$SRC_DIR/include/models/value-view-formatters \
    $$SRC_DIR/include/network \
    $$SRC_DIR/include/redis \
    $$SRC_DIR/include/updater \
    $$SRC_DIR/include/widgets \

FORMS += \
    $$SRC_DIR/forms/*.ui \

RESOURCES += \
    $$SRC_DIR/Resources/demo.qrc


OTHER_FILES += \
    stubs/connections.xml
