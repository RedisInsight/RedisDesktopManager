CONSOLE_SRC_DIR = $$PWD/../../../../src/modules/console/

HEADERS  += \
    $$files($$PWD/*.h) \
    $$CONSOLE_SRC_DIR/consolemodel.h \   


SOURCES += \
    $$files($$PWD/*.cpp) \
    $$CONSOLE_SRC_DIR/consolemodel.cpp \
