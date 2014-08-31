
APP_SRC_DIR = $$PWD/../../../src/app/

INCLUDEPATH += $$APP_SRC_DIR

HEADERS  += \
    $$PWD/*.h \
    $$PWD/mocks/*.h \
    $$APP_SRC_DIR/models/connectionsmanager.h \
    #$$APP_SRC_DIR/models/treeoperations.h \

SOURCES += \
    $$PWD/*.cpp \
    $$APP_SRC_DIR/models/connectionsmanager.cpp \
    #$$APP_SRC_DIR/models/treeoperations.cpp \

OTHER_FILES += \
    connections.xml
