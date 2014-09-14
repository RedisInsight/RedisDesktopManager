
APP_SRC_DIR = $$PWD/../../../src/app/

INCLUDEPATH += $$APP_SRC_DIR

HEADERS  += \
    $$PWD/*.h \
    $$PWD/mocks/*.h \
    $$APP_SRC_DIR/models/connectionsmanager.h \
    $$APP_SRC_DIR/models/treeoperations.h \
    $$APP_SRC_DIR/models/key-models/keyfactory.h \
    $$APP_SRC_DIR/models/key-models/abstractkey.h \
    $$APP_SRC_DIR/models/key-models/stringkey.h \

SOURCES += \
    $$PWD/*.cpp \
    $$APP_SRC_DIR/models/connectionsmanager.cpp \
    $$APP_SRC_DIR/models/treeoperations.cpp \
    $$APP_SRC_DIR/models/key-models/keyfactory.cpp \
    $$APP_SRC_DIR/models/key-models/abstractkey.cpp \
    $$APP_SRC_DIR/models/key-models/stringkey.cpp \

OTHER_FILES += \
    connections.xml
