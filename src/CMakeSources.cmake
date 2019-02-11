set(SERVER
        server/IClient.hpp
        server/IServer.hpp
        server/asio/AsioClient.hpp
        server/asio/AsioServer.hpp
        server/buffer.hpp
        server/DlWrapper.hpp
        server/ModulesManager.hpp
        server/Modules/TestModule.cpp
        )

set(API server/api/AModulesManager.hpp
        server/api/Heading.hpp
        server/api/Stage.hpp
        )

set(UTILS
        Utils/Logger.hpp
        )

set(MAIN
        ../app/main.cpp
        )

set(SRCS
        ${SERVER}
        ${UTILS}
        ${API}
        ${MAIN}
        )
