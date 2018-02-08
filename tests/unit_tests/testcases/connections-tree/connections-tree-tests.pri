
CONNECTIONS_TREE_SRC_DIR = $$PWD/../../../../src/modules/connections-tree/

HEADERS  += \
    $$PWD/*.h \
    $$PWD/mocks/*.h \
    $$CONNECTIONS_TREE_SRC_DIR/items/*.h \    
    $$CONNECTIONS_TREE_SRC_DIR/operations.h \
    $$CONNECTIONS_TREE_SRC_DIR/utils.h \
    $$CONNECTIONS_TREE_SRC_DIR/keysrendering.h \
    $$CONNECTIONS_TREE_SRC_DIR/model.h \

SOURCES += \
    $$PWD/*.cpp \
    $$CONNECTIONS_TREE_SRC_DIR/items/*.cpp \    
    $$CONNECTIONS_TREE_SRC_DIR/utils.cpp \
    $$CONNECTIONS_TREE_SRC_DIR/keysrendering.cpp \
    $$CONNECTIONS_TREE_SRC_DIR/model.cpp \

OTHER_FILES += \
    connections.xml
