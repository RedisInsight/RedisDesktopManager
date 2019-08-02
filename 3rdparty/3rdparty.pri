#-------------------------------------------------
#
# Redis Desktop Manager Dependencies
#
#-------------------------------------------------

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
    LIBS += -L$$PWD/qredisclient/ -lqredisclient -lbotan-2 -lssh2 -lz
    include($$PWD/qredisclient/3rdparty/asyncfuture/asyncfuture.pri)
} else {
    message("Using qredisclient source code")
    include($$PWD/qredisclient/qredisclient.pri)
}


#PyOtherSide
include($$PWD/pyotherside.pri)


win32* {
    ZLIBDIR = $$PWD/zlib-msvc14-x64.1.2.11.7795/build/native
    INCLUDEPATH += $$ZLIBDIR/include
    LIBS += $$ZLIBDIR/lib_release/zlibstatic.lib
}

unix:macx { # OSX
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += /System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices
    LIBS += -lz
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
}
