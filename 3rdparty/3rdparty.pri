#-------------------------------------------------
#
# Redis Desktop Manager Dependencies
#
#-------------------------------------------------

OTHER_FILES += $$PWD/../src/resources/qml/3rdparty/php-unserialize-js/phpUnserialize.js

# qredisclient
include($$PWD/qredisclient/qredisclient.pri)

#qgamp
include($$PWD/qgamp/qgamp.pri)
DEFINES += GMP_ID=\\\"UA-68484170-1\\\"

# Easylogging
INCLUDEPATH += $$PWD/easyloggingpp/src
HEADERS += $$PWD/easyloggingpp/src/easylogging++.h


# Google breakpad
BREAKPADDIR = $$PWD/gbreakpad/src
DEPENDPATH += $$BREAKPADDIR

INCLUDEPATH += $$BREAKPADDIR/
INCLUDEPATH += $$BREAKPADDIR/src

#breakpad app need debug info inside binaries
win32-msvc* {
    QMAKE_CXXFLAGS += /MP
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
} else {
    QMAKE_CXXFLAGS+=-g
    QMAKE_CFLAGS_RELEASE+=-g
}

win32* {    
    win32-g++ {
        # Workaround for mingw
        QMAKE_LFLAGS_RELEASE=
    } else {
        INCLUDEPATH += $$PWD/qredisclient/3rdparty/windows/rmt_zlib.1.2.8.5/build/native/include
    }

    HEADERS += $$BREAKPADDIR/common/windows/string_utils-inl.h
    HEADERS += $$BREAKPADDIR/common/windows/guid_string.h
    HEADERS += $$BREAKPADDIR/client/windows/handler/exception_handler.h
    HEADERS += $$BREAKPADDIR/client/windows/common/ipc_protocol.h
    HEADERS += $$BREAKPADDIR/google_breakpad/common/minidump_format.h
    HEADERS += $$BREAKPADDIR/google_breakpad/common/breakpad_types.h
    HEADERS += $$BREAKPADDIR/client/windows/crash_generation/crash_generation_client.h
    HEADERS += $$BREAKPADDIR/common/scoped_ptr.h
    SOURCES += $$BREAKPADDIR/client/windows/handler/exception_handler.cc
    SOURCES += $$BREAKPADDIR/common/windows/string_utils.cc
    SOURCES += $$BREAKPADDIR/common/windows/guid_string.cc
    SOURCES += $$BREAKPADDIR/client/windows/crash_generation/crash_generation_client.cc
}

unix:macx { # OSX
    PRE_TARGETDEPS += $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework
    LIBS += $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework/Versions/A/Breakpad
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices

    #deployment
    APP_DATA_FILES.files = $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework
    APP_DATA_FILES.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += APP_DATA_FILES
}

unix:!macx { # ubuntu & debian
    QMAKE_CXXFLAGS += -std=gnu++0x #workaround for google breakpad

    # clean default flags
    QMAKE_LFLAGS_RPATH=

    LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib #don't remove!!!
    LIBS += $$BREAKPADDIR/client/linux/libbreakpad_client.a
}
