#-------------------------------------------------
#
# Redis Desktop Manager
#
#-------------------------------------------------

QT += core gui network concurrent widgets quick quickwidgets charts

TARGET = rdm
TEMPLATE = app

# Skip version file
!exists( $$PWD/version.h ) {    
    DEFINES += RDM_VERSION=\\\"0.9.999\\\"
}

DEFINES += CORE_LIBRARY ELPP_QT_LOGGING ELPP_STL_LOGGING ELPP_DISABLE_DEFAULT_CRASH_HANDLING

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/app/app.cpp \
    $$PWD/app/qmlutils.cpp \
    $$PWD/app/models/*.cpp \
    $$PWD/app/models/key-models/*.cpp \
    $$PWD/modules/connections-tree/*.cpp \
    $$PWD/modules/connections-tree/items/*.cpp \
    $$PWD/modules/console/*.cpp \
    $$PWD/modules/value-editor/*.cpp \
    $$PWD/modules/crashhandler/*.cpp \
    $$PWD/modules/updater/*.cpp \
    $$PWD/modules/bulk-operations/*.cpp \
    $$PWD/modules/common/*.cpp \
    $$PWD/modules/server-stats/*.cpp \

HEADERS  += \
    $$PWD/app/app.h \
    $$PWD/app/logger.h \
    $$PWD/app/qmlutils.h \
    $$PWD/app/models/*.h \
    $$PWD/app/models/key-models/*.h \
    $$PWD/modules/connections-tree/*.h \
    $$PWD/modules/connections-tree/items/*.h \
    $$PWD/modules/console/*.h \
    $$PWD/modules/value-editor/*.h \
    $$PWD/modules/crashhandler/*.h \
    $$PWD/modules/updater/*.h \
    $$PWD/modules/*.h \
    $$PWD/modules/bulk-operations/*.h \
    $$PWD/modules/common/*.h \
    $$PWD/modules/server-stats/*.h \

exists( $$PWD/version.h ) {
    HEADERS  += $$PWD/version.h
}

THIRDPARTYDIR = $$PWD/../3rdparty/

include($$THIRDPARTYDIR/3rdparty.pri)

win32 {
    CONFIG += c++11
    RC_FILE += $$PWD/resources/rdm.rc

    win32-msvc* {
        QMAKE_LFLAGS += /LARGEADDRESSAWARE
    }

    release: DESTDIR = ./../bin/windows/release
    debug:   DESTDIR = ./../bin/windows/debug
}

unix:macx { # OSX
    QT += svg
    CONFIG += c++11

    debug: CONFIG-=app_bundle

    release: DESTDIR = ./../bin/osx/release
    debug:   DESTDIR = ./../bin/osx/debug

    #deployment
    QMAKE_INFO_PLIST =  $$PWD/resources/Info.plist
    ICON = $$PWD/resources/rdm.icns

    release {
        CRASHREPORTER_APP.files = $$DESTDIR/crashreporter
        CRASHREPORTER_APP.path = Contents/MacOS
        QMAKE_BUNDLE_DATA += CRASHREPORTER_APP
    }
}

unix:!macx { # ubuntu & debian
    CONFIG += static release
    CONFIG -= debug    

    QTPLUGIN += qsvg qsvgicon

    QMAKE_CXXFLAGS += -Wno-sign-compare    

    release: DESTDIR = $$PWD/../bin/linux/release
    debug:   DESTDIR = $$PWD/../bin/linux/debug

    #deployment
    LINUX_INSTALL_PATH = /opt/redis-desktop-manager
    
    target.path = $$LINUX_INSTALL_PATH
    target.files = $$DESTDIR/rdm $$DESTDIR/crashreporter $$PWD/resources/rdm.sh
    INSTALLS += target
    
    exists( $$PWD/resources/qt.conf ) {
       appconfig.path = $$LINUX_INSTALL_PATH
       appconfig.files = $$PWD/resources/qt.conf
       INSTALLS += appconfig
    }
    
    data.path = $$LINUX_INSTALL_PATH/lib
    data.files = $$PWD/lib/*
    INSTALLS += data

    appicon.path = /usr/share/pixmaps/
    appicon.files = $$PWD/resources/rdm.png
    INSTALLS += appicon

    deskicon.path = /usr/share/applications
    deskicon.files =  $$PWD/resources/rdm.desktop
    INSTALLS += deskicon
}

UI_DIR = $$DESTDIR/ui
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/obj

INCLUDEPATH += $$PWD/ \
    $$PWD/modules/ \
    $$UI_DIR/ \

RESOURCES += \
    $$PWD/resources/images.qrc \
    $$PWD/resources/fonts.qrc \    
    $$PWD/qml/qml.qrc \
    $$PWD/resources/tr.qrc \

OTHER_FILES += \
    qt.conf \
    Info.plist \
    qml\*.qml \


lupdate_only{
    SOURCES += \
        $$PWD/qml/*.qml \
        $$PWD/qml/value-editor/*.qml \
        $$PWD/qml/settings/*.qml \
        $$PWD/qml/server-info/*.qml \
        $$PWD/qml/console/*.qml \
        $$PWD/qml/connections-tree/*.qml \
        $$PWD/qml/common/*.qml \
        $$PWD/qml/bulk-operations/*.qml \
}


TRANSLATIONS = \
    $$PWD/resources/translations/rdm.ts \
    $$PWD/resources/translations/rdm_zh_CN.ts \
    $$PWD/resources/translations/rdm_zh_TW.ts

CODECFORSRC = UTF-8
