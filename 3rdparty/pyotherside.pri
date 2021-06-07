
# Python
PY_VERSION="39"
PY_WIN_VERSION="38"
PY_LIB_SUFFIX="3.9"

win32* {
    QMAKE_LIBS += -LC:\Python$${PY_WIN_VERSION}-x64\libs -lpython$${PY_WIN_VERSION}
    INCLUDEPATH += C:\Python$${PY_WIN_VERSION}-x64\include\
} else {
    unix:macx {
      exists($$PWD/python-3) {
        message("Using Python from 3rdparty dir")
        LIBS += $$PWD/python-3/lib/libpython$${PY_LIB_SUFFIX}.dylib
        INCLUDEPATH += $$PWD/python-3/include/python$${PY_LIB_SUFFIX}

        #deployment
        PY_DATA_FILES.files = $$PWD/python-3/lib/libpython$${PY_LIB_SUFFIX}.dylib
        PY_DATA_FILES.path = Contents/Frameworks
        QMAKE_BUNDLE_DATA += PY_DATA_FILES

      } else {
       PYTHON_CONFIG = /usr/local/bin/python3-config
       QMAKE_LIBS += $$system($$PYTHON_CONFIG --ldflags --libs --embed)
       QMAKE_CXXFLAGS += $$system($$PYTHON_CONFIG --includes)
      }
    } else {
      PYTHON_CONFIG = python3-config

      PYTHON_VERSION = $$str_member($$system(python3 --version), 7, 11)
      message("Python version $$PYTHON_VERSION")

      versionAtLeast(PYTHON_VERSION, "3.8.0") {        
        QMAKE_LIBS += $$system($$PYTHON_CONFIG --ldflags --libs --embed)
      } else {
        QMAKE_LIBS += $$system($$PYTHON_CONFIG --ldflags --libs)
      }

      QMAKE_CXXFLAGS += $$system($$PYTHON_CONFIG --includes)
      DEFINES *= HAVE_DLADDR
    }
}

include(pyotherside/pyotherside.pri)

DEFINES += PYOTHERSIDE_VERSION=\\\"$${VERSION}\\\"

DEPENDPATH += $$PWD/pyotherside/src
INCLUDEPATH += $$PWD/pyotherside/src

PYOTHERSIDE_DIR = $$PWD/pyotherside/src/

# Importer from Qt Resources
RESOURCES += $$PYOTHERSIDE_DIR/qrc_importer.qrc

HEADERS += $$PYOTHERSIDE_DIR/pythonlib_loader.h\
    $$PWD/pyotherside/src/callback.h
SOURCES += $$PYOTHERSIDE_DIR/pythonlib_loader.cpp

# Python QML Object
SOURCES += $$PYOTHERSIDE_DIR/qpython.cpp
HEADERS += $$PYOTHERSIDE_DIR/qpython.h
SOURCES += $$PYOTHERSIDE_DIR/qpython_worker.cpp
HEADERS += $$PYOTHERSIDE_DIR/qpython_worker.h
SOURCES += $$PYOTHERSIDE_DIR/qpython_priv.cpp
HEADERS += $$PYOTHERSIDE_DIR/qpython_priv.h
HEADERS += $$PYOTHERSIDE_DIR/python_wrap.h

# Globally Load Python hack
SOURCES += $$PYOTHERSIDE_DIR/global_libpython_loader.cpp
HEADERS += $$PYOTHERSIDE_DIR/global_libpython_loader.h

# Reference-counting PyObject wrapper class
SOURCES += $$PYOTHERSIDE_DIR/pyobject_ref.cpp
HEADERS += $$PYOTHERSIDE_DIR/pyobject_ref.h

# QObject wrapper class exposed to Python
SOURCES += $$PYOTHERSIDE_DIR/qobject_ref.cpp
HEADERS += $$PYOTHERSIDE_DIR/qobject_ref.h
HEADERS += $$PYOTHERSIDE_DIR/pyqobject.h

# GIL helper
HEADERS += $$PYOTHERSIDE_DIR/ensure_gil_state.h

# Type System Conversion Logic
HEADERS += $$PYOTHERSIDE_DIR/converter.h
HEADERS += $$PYOTHERSIDE_DIR/qvariant_converter.h
HEADERS += $$PYOTHERSIDE_DIR/pyobject_converter.h
HEADERS += $$PYOTHERSIDE_DIR/qml_python_bridge.h
