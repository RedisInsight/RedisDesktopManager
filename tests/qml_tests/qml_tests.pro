TEMPLATE = app
TARGET = qml_tests
CONFIG += warn_on qmltestcase
SOURCES += $$PWD/qml_test_runner.cpp

PROJECT_ROOT = $$PWD/../..//

release: DESTDIR = $$PROJECT_ROOT/bin/tests
debug:   DESTDIR = $$PROJECT_ROOT/bin/tests

OBJECTS_DIR = $$DESTDIR/qml_obj
MOC_DIR = $$DESTDIR/qml_obj
RCC_DIR = $$DESTDIR/qml_obj

OTHER_FILES = $$PWD/tst_*.qml
