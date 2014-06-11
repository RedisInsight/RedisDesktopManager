#-------------------------------------------------
#
# Redis Desktop Manager Dependencies
#
#-------------------------------------------------

BREAKPADDIR = $$PWD/breakpad/src

INCLUDEPATH += $$PWD/libssh2/include
DEPENDPATH += $$PWD/libssh2/include
DEPENDPATH += $$BREAKPADDIR

win32-msvc* {
    INCLUDEPATH += $$BREAKPADDIR\client\windows\handler\
    INCLUDEPATH += $$BREAKPADDIR\common\windows\
    INCLUDEPATH += $$BREAKPADDIR\client\windows\crash_generation
    INCLUDEPATH += $$BREAKPADDIR\client\windows\common\
    INCLUDEPATH += $$BREAKPADDIR\google_breakpad\common\
    INCLUDEPATH += $$BREAKPADDIR\processor
    INCLUDEPATH += $$BREAKPADDIR\

    CONFIG(release, debug|release) {
        WIN_DEPS_PATH = $$PWD/libs/win32/release/
    } else: CONFIG(debug, debug|release) {
        WIN_DEPS_PATH = $$PWD/libs/win32/debug/
    }

    LIBS += -L$$WIN_DEPS_PATH -llibssh2
    LIBS += -lcommon -lcrash_generation_client -lexception_handler

    CONFIG(debug, debug|release) {
        LIBS += -llibeay32 -lssleay32 -lzlib
    }

    PRE_TARGETDEPS += $$WIN_DEPS_PATH/libssh2.lib

    QMAKE_CXXFLAGS += /MP
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
}

unix:macx { # OSX

    PRE_TARGETDEPS += /usr/local/lib/libssh2.dylib \
                     $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework

    LIBS += /usr/local/lib/libssh2.dylib
    LIBS += $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework/Versions/A/Breakpad
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices

    INCLUDEPATH += $$PWD/libssh/include
    INCLUDEPATH += $$BREAKPADDIR/ \
                    $$BREAKPADDIR/client/mac/handler/ \
                    $$BREAKPADDIR/client/mac/ \
                    $$BREAKPADDIR/client/ \
                    $$BREAKPADDIR/client/mac/crash_generation/ \
                    $$BREAKPADDIR/common/mac/ \
                    $$BREAKPADDIR/common/linux/ \
                    $$BREAKPADDIR/common/ \
                    $$BREAKPADDIR/processor/ \
                    $$BREAKPADDIR/google_breakpad/common/ \
                    $$BREAKPADDIR/third_party/lss/ \

    #breakpad app need debug info inside binaries
    QMAKE_CXXFLAGS+=-g

    #deployment
    APP_DATA_FILES.files = $$BREAKPADDIR/client/mac/build/Release/Breakpad.framework
    APP_DATA_FILES.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += APP_DATA_FILES
}

unix:!macx { # ubuntu & debian

    QMAKE_CXXFLAGS += -std=gnu++0x -g #workaround for google breakpad

    LIBS += -Wl,-rpath=\\\$$ORIGIN/../lib #don't remove!!!
    LIBS += /usr/local/lib/libssh2.a \
            /usr/local/lib/libbreakpad.a \
            /usr/local/lib/libbreakpad_client.a \

    PRE_TARGETDEPS +=/usr/local/lib/libssh2.a \
                     /usr/local/lib/libbreakpad.a \
                     /usr/local/lib/libbreakpad_client.a \

    INCLUDEPATH += $$BREAKPADDIR/ \
        $$BREAKPADDIR/client/linux/handler/ \
        $$BREAKPADDIR/client/linux/crash_generation/ \
        $$BREAKPADDIR/client/linux/minidump_writer/ \
        $$BREAKPADDIR/client/linux/log/ \
        $$BREAKPADDIR/client/ \
        $$BREAKPADDIR/common/linux/ \
        $$BREAKPADDIR/common/ \
        $$BREAKPADDIR/google_breakpad/common/ \
        $$BREAKPADDIR/processor/ \
        $$BREAKPADDIR/third_party/lss/ \

    DEPENDPATH += $$BREAKPADDIR/ \
        $$BREAKPADDIR/client/linux/handler/ \
        $$BREAKPADDIR/client/linux/crash_generation/ \
        $$BREAKPADDIR/client/linux/minidump_writer/ \
        $$BREAKPADDIR/client/linux/log/ \
        $$BREAKPADDIR/client/ \
        $$BREAKPADDIR/common/linux/ \
        $$BREAKPADDIR/common/ \
        $$BREAKPADDIR/google_breakpad/common/ \
        $$BREAKPADDIR/processor/ \
        $$BREAKPADDIR/third_party/lss/ \
}
