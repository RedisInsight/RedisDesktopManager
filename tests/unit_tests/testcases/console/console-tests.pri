CONSOLE_SRC_DIR = $$PWD/../../../../src/modules/console/

HEADERS  += \
    $$PWD/*.h \  
    $$CONSOLE_SRC_DIR/consolemodel.h \
    $$CONSOLE_SRC_DIR/hex_utils.h \    


SOURCES += \
    $$PWD/*.cpp \    
    $$CONSOLE_SRC_DIR/consolemodel.cpp \
    $$CONSOLE_SRC_DIR/hex_utils.cpp \
