QT += network

HEADERS += \
# Models
    $${PWD}/OAIDataFormatter.h \
    $${PWD}/OAIDecodePayload.h \
    $${PWD}/OAIEncodePayload.h \
    $${PWD}/OAIInline_response_400.h \
# APIs
    $${PWD}/OAIDefaultApi.h \
# Others
    $${PWD}/OAIHelpers.h \
    $${PWD}/OAIHttpRequest.h \
    $${PWD}/OAIObject.h \
    $${PWD}/OAIEnum.h \
    $${PWD}/OAIHttpFileElement.h \
    $${PWD}/OAIServerConfiguration.h \
    $${PWD}/OAIServerVariable.h \
    $${PWD}/OAIOauth.h

SOURCES += \
# Models
    $${PWD}/OAIDataFormatter.cpp \
    $${PWD}/OAIDecodePayload.cpp \
    $${PWD}/OAIEncodePayload.cpp \
    $${PWD}/OAIInline_response_400.cpp \
# APIs
    $${PWD}/OAIDefaultApi.cpp \
# Others
    $${PWD}/OAIHelpers.cpp \
    $${PWD}/OAIHttpRequest.cpp \
    $${PWD}/OAIHttpFileElement.cpp \
    $${PWD}/OAIOauth.cpp
