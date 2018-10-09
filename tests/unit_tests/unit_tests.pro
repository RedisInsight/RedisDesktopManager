QT       += core gui network concurrent widgets quick quickwidgets testlib

TARGET = tests
TEMPLATE = app

CONFIG += debug c++11
CONFIG-=app_bundle 

PROJECT_ROOT = $$PWD/../..//
SRC_DIR = $$PROJECT_ROOT/src//

HEADERS += \
    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/basetestcase.h \    
    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/mocks/*.h \
    $$PROJECT_ROOT/src/modules/common/*.h \

SOURCES += \
    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/basetestcase.cpp \
    $$PROJECT_ROOT/src/modules/common/*.cpp \
    $$PWD/main.cpp \


#RESOURCES += \
#    $$SRC_DIR/resources/rdm.qrc

INCLUDEPATH += $$SRC_DIR/modules/ \
    $$SRC_DIR/ \
    $$PWD/ \
    $$PROJECT_ROOT/3rdparty/qredisclient/tests/unit_tests/

DEFINES += INTEGRATION_TESTS

#TEST CASES
include($$PWD/testcases/app/app-tests.pri)
include($$PWD/testcases/connections-tree/connections-tree-tests.pri)
include($$PWD/testcases/console/console-tests.pri)
include($$PWD/testcases/value-editor/value-editor-tests.pri)
#############
include($$PROJECT_ROOT/3rdparty/3rdparty.pri)

HEADERS += $$PROJECT_ROOT/3rdparty/qt_modeltest/*.h
SOURCES += $$PROJECT_ROOT/3rdparty/qt_modeltest/*.cpp
INCLUDEPATH += $$PROJECT_ROOT/3rdparty/qt_modeltest/

release: DESTDIR = $$PROJECT_ROOT/bin/tests
debug:   DESTDIR = $$PROJECT_ROOT/bin/tests

unix:!mac {
    #code coverage
    QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 -Wno-sign-compare
    QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
    LIBS += -lgcov
}

UI_DIR = $$DESTDIR/ui
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/obj
RCC_DIR = $$DESTDIR/obj
