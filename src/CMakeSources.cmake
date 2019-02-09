set(SERVER
        server/IClient.hpp
        server/IServer.hpp
        server/asio/AsioClient.hpp
        server/asio/AsioServer.hpp
        server/buffer.hpp
        server/ModulesManager.hpp
        server/DlWrapper.hpp
        server/api/AModulesManager.hpp
        server/api/Heading.hpp
        server/api/Stage.hpp
        )

set(HEADER
        header/HeaderEnum.hpp
        header/ResponseHeading.hpp)

set(UTILS
        Utils/Logger.hpp
        )

set(MAIN
        ../app/main.cpp
        )

set(SRCS
        ${SERVER}
        ${HEADER}
        ${UTILS}
        ${MAIN}
        )
