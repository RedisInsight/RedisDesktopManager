
CONNECTIONS_TREE_SRC_DIR = $$PWD/../../../../src/modules/connections-tree/

HEADERS  += \
    $$PWD/mocks.h \
    $$files($$PWD/test_*.h) \
    $$files($$CONNECTIONS_TREE_SRC_DIR/items/*.h) \
    $$CONNECTIONS_TREE_SRC_DIR/operations.h \
    $$CONNECTIONS_TREE_SRC_DIR/utils.h \
    $$CONNECTIONS_TREE_SRC_DIR/keysrendering.h \
    $$CONNECTIONS_TREE_SRC_DIR/model.h \

SOURCES += \
    $$PWD/mocks.cpp \
    $$files($$PWD/test_*.cpp) \
    $$files($$CONNECTIONS_TREE_SRC_DIR/items/*.cpp) \
    $$CONNECTIONS_TREE_SRC_DIR/utils.cpp \
    $$CONNECTIONS_TREE_SRC_DIR/keysrendering.cpp \
    $$CONNECTIONS_TREE_SRC_DIR/model.cpp \
