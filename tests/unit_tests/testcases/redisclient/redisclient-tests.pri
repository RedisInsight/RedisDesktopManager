
REDISCLIENT_SRC_DIR = $$PWD/../../../../src/modules/redisclient/

HEADERS  += \
    $$PWD/*.h \
    $$PWD/mocks/*.h \
    $$REDISCLIENT_SRC_DIR/*.h \
    $$REDISCLIENT_SRC_DIR/ssh/*.h \
    $$REDISCLIENT_SRC_DIR/transporters/*.h \

SOURCES += \
    $$PWD/*.cpp \
    $$REDISCLIENT_SRC_DIR/*.cpp \
    $$REDISCLIENT_SRC_DIR/ssh/*.cpp \
    $$REDISCLIENT_SRC_DIR/transporters/*.cpp \

OTHER_FILES += \
    connections.xml
