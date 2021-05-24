#-------------------------------------------------
#
# Redis Desktop Manager
#
#-------------------------------------------------

CCACHE_BIN = $$system(which ccache)
!isEmpty(CCACHE_BIN) {
  load(ccache)
  CONFIG+=ccache
}

QT += core gui network concurrent widgets quick quickwidgets charts svg

TARGET = rdm
TEMPLATE = app

!defined(VERSION, var) {
    VERSION=2021.3.0-dev
}

message($$VERSION)
DEFINES += RDM_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/app/app.cpp \
    $$PWD/app/apputils.cpp \
    $$PWD/app/events.cpp \
    $$PWD/app/qmlutils.cpp \
    $$PWD/app/qcompress.cpp \
    $$files($$PWD/app/models/*.cpp) \
    $$files($$PWD/app/models/key-models/*.cpp) \
    $$files($$PWD/modules/connections-tree/*.cpp) \
    $$files($$PWD/modules/connections-tree/items/*.cpp) \
    $$files($$PWD/modules/console/*.cpp) \
    $$files($$PWD/modules/value-editor/*model.cpp) \
    $$files($$PWD/modules/value-editor/embedded*.cpp) \
    $$files($$PWD/modules/value-editor/textcharformat.cpp) \
    $$files($$PWD/modules/value-editor/syntaxhighlighter.cpp) \
    $$files($$PWD/modules/bulk-operations/*.cpp) \
    $$files($$PWD/modules/bulk-operations/operations/*.cpp) \
    $$files($$PWD/modules/common/*.cpp) \
    $$files($$PWD/modules/server-stats/*.cpp) \

HEADERS  += \
    $$PWD/app/app.h \
    $$PWD/app/events.h \
    $$PWD/app/apputils.h \
    $$PWD/app/qmlutils.h \
    $$PWD/app/qcompress.h \
    $$PWD/app/darkmode.h \
    $$files($$PWD/app/models/*.h) \
    $$files($$PWD/app/models/key-models/*.h) \
    $$files($$PWD/modules/connections-tree/*.h) \
    $$files($$PWD/modules/connections-tree/items/*.h) \
    $$files($$PWD/modules/console/*.h) \
    $$files($$PWD/modules/value-editor/*factory.h) \
    $$files($$PWD/modules/value-editor/*model.h) \
    $$files($$PWD/modules/value-editor/embedded*.h) \
    $$files($$PWD/modules/value-editor/textcharformat.h) \
    $$files($$PWD/modules/value-editor/syntaxhighlighter.h) \
    $$files($$PWD/modules/*.h) \
    $$files($$PWD/modules/bulk-operations/*.h) \
    $$files($$PWD/modules/bulk-operations/operations/*.h) \
    $$files($$PWD/modules/common/*.h) \
    $$files($$PWD/modules/server-stats/*.h) \

THIRDPARTYDIR = $$PWD/../3rdparty/

include($$THIRDPARTYDIR/3rdparty.pri)

exists( $$PWD/modules/crashpad/crashpad.pri ) {
    message("Build with Crashpad")
    include($$PWD/modules/crashpad/crashpad.pri)
}

release {
    message("Enable qtquickcompiler")
    CONFIG += qtquickcompiler
}

win32 {
    CONFIG += c++11

    RC_ICONS = $$PWD/resources/images/logo.ico
    QMAKE_TARGET_COMPANY = rdm.dev
    QMAKE_TARGET_PRODUCT = RDM
    QMAKE_TARGET_DESCRIPTION = "Open source GUI management tool for Redis®"
    QMAKE_TARGET_COPYRIGHT = "Igor Malinovskiy (C) 2013-2020"

    release: DESTDIR = ./../bin/windows/release
    debug:   DESTDIR = ./../bin/windows/debug
}

unix:macx { # OSX
    TARGET = "RDM"
    QT += svg
    CONFIG += c++11

    debug: CONFIG-=app_bundle

    release: DESTDIR = ./../bin/osx/release
    debug:   DESTDIR = ./../bin/osx/debug

    #deployment
    QMAKE_INFO_PLIST =  $$PWD/resources/Info.plist
    ICON = $$PWD/resources/rdm.icns
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
    appicon.files = $$PWD/resources/images/rdm.png
    INSTALLS += appicon

    deskicon.path = /usr/share/applications
    deskicon.files =  $$PWD/resources/rdm.desktop
    INSTALLS += deskicon

    RESOURCES += $$PWD/resources/fonts.qrc
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
    $$PWD/qml/qml.qrc \
    $$PWD/py/py.qrc \
    $$PWD/resources/commands.qrc

exists( $$PWD/resources/translations/rdm.qm ) {
    message("Translations found")
    RESOURCES += $$PWD/resources/tr.qrc
}

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
    $$PWD/resources/translations/rdm_zh_TW.ts \
    $$PWD/resources/translations/rdm_ru_RU.ts \
    $$PWD/resources/translations/rdm_es_ES.ts \
    $$PWD/resources/translations/rdm_ja_JP.ts \
    $$PWD/resources/translations/rdm_uk_UA.ts \

CODECFORSRC = UTF-8
