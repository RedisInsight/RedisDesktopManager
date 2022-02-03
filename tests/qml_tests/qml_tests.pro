TEMPLATE = app
TARGET = qml_tests

CONFIG += warn_on qmltestcase
CONFIG-=app_bundle

SOURCES += $$PWD/qml_test_runner.cpp \
    setup.cpp

PROJECT_ROOT = $$PWD/../..//

INCLUDEPATH += $$PROJECT_ROOT/src $$PROJECT_ROOT/src/modules $$PROJECT_ROOT/3rdparty/qredisclient/src
SOURCES += \
    $$files($$PROJECT_ROOT/src/app/apputils.cpp) \
    $$files($$PROJECT_ROOT/src/app/qmlutils.cpp) \
    $$files($$PROJECT_ROOT/src/app/jsonutils.cpp) \
    $$files($$PROJECT_ROOT/src/app/qcompress.cpp) \
    $$files($$PROJECT_ROOT/src/modules/value-editor/textcharformat.cpp) \
    $$files($$PROJECT_ROOT/src/modules/value-editor/syntaxhighlighter.cpp) \
    $$files($$PROJECT_ROOT/src/modules/value-editor/largetextmodel.cpp) \

QT += core gui quick network concurrent charts

release: DESTDIR = $$PROJECT_ROOT/bin/tests
debug:   DESTDIR = $$PROJECT_ROOT/bin/tests

OBJECTS_DIR = $$DESTDIR/qml_obj
MOC_DIR = $$DESTDIR/qml_obj
RCC_DIR = $$DESTDIR/qml_obj

OTHER_FILES = $$PWD/tst_*.qml

HEADERS += \
    setup.h \
    $$files($$PROJECT_ROOT/src/app/qmlutils.h) \
    $$files($$PROJECT_ROOT/src/modules/value-editor/textcharformat.h) \
    $$files($$PROJECT_ROOT/src/modules/value-editor/syntaxhighlighter.h) \
    $$files($$PROJECT_ROOT/src/modules/value-editor/largetextmodel.h) \

DISTFILES += \
    tst_MultilineEditor.qml

include($$PROJECT_ROOT/3rdparty/3rdparty.pri)
