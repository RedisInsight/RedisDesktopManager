CONSOLE_SRC_DIR = $$PWD/../../../src/modules/console/

HEADERS  += \
    $$PWD/*.h \
    $$PWD/mocks/*.h \
    $$CONSOLE_SRC_DIR/*.h \
    $$CONSOLE_SRC_DIR/../basetab.h \


SOURCES += \
    $$PWD/*.cpp \
    $$CONSOLE_SRC_DIR/*.cpp \
