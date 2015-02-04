CONSOLE_SRC_DIR = $$PWD/../../../../src/modules/console/

HEADERS  += \
    $$PWD/*.h \
    $$PWD/mocks/*.h \
    $$CONSOLE_SRC_DIR/consoletab.h \
    $$CONSOLE_SRC_DIR/operations.h \
    $$CONSOLE_SRC_DIR/hex_utils.h \
    $$CONSOLE_SRC_DIR/../basetab.h \


SOURCES += \
    $$PWD/*.cpp \
    $$CONSOLE_SRC_DIR/consoletab.cpp \
    $$CONSOLE_SRC_DIR/hex_utils.cpp \
