QT += network

HEADERS += \
# Models
    $${PWD}/OAIError.h \
    $${PWD}/OAIItem.h \
# APIs
    $${PWD}/OAITodosApi.h \
# Others
    $${PWD}/OAIHelpers.h \
    $${PWD}/OAIHttpRequest.h \
    $${PWD}/OAIObject.h \
    $${PWD}/OAIEnum.h \
    $${PWD}/OAIHttpFileElement.h \
    $${PWD}/OAIServerConfiguration.h \
    $${PWD}/OAIServerVariable.h

SOURCES += \
# Models
    $${PWD}/OAIError.cpp \
    $${PWD}/OAIItem.cpp \
# APIs
    $${PWD}/OAITodosApi.cpp \
# Others
    $${PWD}/OAIHelpers.cpp \
    $${PWD}/OAIHttpRequest.cpp \
    $${PWD}/OAIHttpFileElement.cpp

