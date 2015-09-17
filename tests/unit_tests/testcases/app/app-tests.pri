
APP_SRC_DIR = $$PWD/../../../../src/app/

INCLUDEPATH += $$APP_SRC_DIR

HEADERS  += \
    $$PWD/test_*.h \
    $$APP_SRC_DIR/models/connectionsmanager.h \
    $$APP_SRC_DIR/models/configmanager.h \
    $$APP_SRC_DIR/models/connectionconf.h \
    $$APP_SRC_DIR/models/treeoperations.h \
    $$APP_SRC_DIR/models/key-models/keyfactory.h \
    $$APP_SRC_DIR/models/key-models/abstractkey.h \
    $$APP_SRC_DIR/models/key-models/stringkey.h \
    $$APP_SRC_DIR/models/key-models/listkey.h \
    $$APP_SRC_DIR/models/key-models/listlikekey.h \
    $$APP_SRC_DIR/models/key-models/setkey.h \
    $$APP_SRC_DIR/models/key-models/sortedsetkey.h \
    $$APP_SRC_DIR/models/key-models/hashkey.h \
    $$APP_SRC_DIR/models/consoleoperations.h \
    $$APP_SRC_DIR/widgets/consoletabs.h \    
    $$APP_SRC_DIR/dialogs/connect.h \

SOURCES += \
    $$PWD/test_*.cpp \
    $$APP_SRC_DIR/models/connectionsmanager.cpp \
    $$APP_SRC_DIR/models/configmanager.cpp \
    $$APP_SRC_DIR/models/connectionconf.cpp \
    $$APP_SRC_DIR/models/treeoperations.cpp \
    $$APP_SRC_DIR/models/key-models/keyfactory.cpp \    
    $$APP_SRC_DIR/models/key-models/stringkey.cpp \
    $$APP_SRC_DIR/models/key-models/listkey.cpp \
    $$APP_SRC_DIR/models/key-models/listlikekey.cpp \
    $$APP_SRC_DIR/models/key-models/setkey.cpp \
    $$APP_SRC_DIR/models/key-models/sortedsetkey.cpp \
    $$APP_SRC_DIR/models/key-models/hashkey.cpp \
    $$APP_SRC_DIR/models/consoleoperations.cpp \
    $$APP_SRC_DIR/widgets/consoletabs.cpp \
    $$APP_SRC_DIR/dialogs/connect.cpp \

OTHER_FILES += \
    connections.xml

FORMS += \
    $$APP_SRC_DIR/forms/connection.ui \

