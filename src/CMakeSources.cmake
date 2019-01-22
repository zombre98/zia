set(SERVER
        server/IClient.hpp
        server/IServer.hpp
        server/asio/AsioClient.hpp
        server/asio/AsioServer.hpp
        server/buffer.hpp
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
