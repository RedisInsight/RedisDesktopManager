#-------------------------------------------------
#
# Redis Desktop Manager Dependencies
#
#-------------------------------------------------

exists( $$_PRO_FILE_PWD_/modules/extension-server/client/client.pri) {
    message("RESP.app Extension server integration was enabled")
    DEFINES += ENABLE_EXTERNAL_FORMATTERS
    HEADERS += $$_PRO_FILE_PWD_/modules/extension-server/dataformattermanager.h
    SOURCES += $$_PRO_FILE_PWD_/modules/extension-server/dataformattermanager.cpp
    include($$_PRO_FILE_PWD_/modules/extension-server/client/client.pri)
}

# qredisclient
if(win32*):exists( $$PWD/qredisclient/qredisclient.lib ) {
    message("Using prebuilt qredisclient")    
    INCLUDEPATH += $$PWD/qredisclient/src/
    OPENSSL_LIB_PATH = C:\OpenSSL-Win64\lib\VC
    LIBS += -L$$OPENSSL_LIB_PATH -llibeay32MD -L$$PWD/qredisclient/ -lqredisclient -lbotan -llibssh2 -lgdi32 -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
    include($$PWD/qredisclient/3rdparty/asyncfuture/asyncfuture.pri)
} else:unix*:exists( $$PWD/qredisclient/libqredisclient.a ) {
    message("Using prebuilt qredisclient")
    INCLUDEPATH += $$PWD/qredisclient/src/
    LIBS += -L$$PWD/qredisclient/ -lqredisclient -lbotan-2 -lssh2 -lz -lssl -lcrypto
    include($$PWD/qredisclient/3rdparty/asyncfuture/asyncfuture.pri)
} else {
    message("Using qredisclient source code")
    include($$PWD/qredisclient/qredisclient.pri)
}


#PyOtherSide
include($$PWD/pyotherside.pri)

#LZ4
LZ4DIR = $$PWD/lz4/
INCLUDEPATH += $$LZ4DIR/lib

#ZSTD
ZSTDDIR = $$PWD/zstd/
INCLUDEPATH += $$ZSTDDIR/lib

#Snappy
SNAPPYDIR = $$PWD/snappy
INCLUDEPATH += $$SNAPPYDIR

#Brotli
BROTLIDIR = $$PWD/brotli
INCLUDEPATH += $$BROTLIDIR/c/include

#SIMDJSON
SIMDJSONDIR = $$PWD/simdjson/singleheader
INCLUDEPATH += $$SIMDJSONDIR/
HEADERS += $$SIMDJSONDIR/simdjson.h
SOURCES += $$SIMDJSONDIR/simdjson.cpp


win32* {
    ZLIBDIR = $$PWD/zlib-msvc14-x64.1.2.11.7795/build/native    
    INCLUDEPATH += $$ZLIBDIR/include
    LIBS += $$ZLIBDIR/lib_release/zlibstatic.lib $$LZ4DIR/build/cmake/Release/lz4.lib
    LIBS += $$ZSTDDIR/build/cmake/lib/Release/zstd_static.lib
    LIBS += $$SNAPPYDIR/Release/snappy.lib
    LIBS += -L$$BROTLIDIR/Release/ -lbrotlicommon-static -lbrotlidec-static -lbrotlienc-static
}

unix:macx { # OSX
    LIBS += -lz $$LZ4DIR/build/cmake/liblz4.a $$ZSTDDIR/build/cmake/lib/libzstd.a
    LIBS += $$SNAPPYDIR/libsnappy.a
    LIBS += -L$$BROTLIDIR/ -lbrotlicommon-static -lbrotlidec-static -lbrotlienc-static
}

unix:!macx { # ubuntu & debian   
    defined(CLEAN_RPATH, var) { # clean default flags
        message("DEB package build")
        QMAKE_LFLAGS_RPATH=
        QMAKE_LFLAGS = -Wl,-rpath=\\\$$ORIGIN/../lib
        QMAKE_LFLAGS += -static-libgcc -static-libstdc++
    } else {
        # Note: uncomment if qtcreator fails to find QtCore dependencies
        #QMAKE_LFLAGS = -Wl,-rpath=/home/user/Qt5.9.3/5.9.3/gcc_64/lib
    }

    LIBS += -lz
    defined(SYSTEM_LZ4, var) {
        LIBS += -llz4
    } else {
        LIBS += $$LZ4DIR/build/cmake/liblz4.a
    }

    defined(SYSTEM_ZSTD, var) {
        LIBS += -lzstd
    } else {
        LIBS += $$ZSTDDIR/build/cmake/lib/libzstd.a
    }

    defined(SYSTEM_SNAPPY, var) {
        LIBS += -lsnappy
    } else {
        LIBS += $$SNAPPYDIR/libsnappy.a
    }

    defined(SYSTEM_BROTLI, var) {
        LIBS += -lbrotlicommon -lbrotlidec -lbrotlienc
    } else {
        LIBS += -L$$BROTLIDIR/ -lbrotlienc-static -lbrotlicommon-static -lbrotlidec-static
    }

    # Unix signal watcher
    defined(LINUX_SIGNALS, var) {
        message("Build with qt-unix-signals")

        DEFINES += LINUX_SIGNALS
        HEADERS += $$PWD/qt-unix-signals/sigwatch.h
        SOURCES += $$PWD/qt-unix-signals/sigwatch.cpp
        INCLUDEPATH += $$PWD/qt-unix-signals/
    }
}
