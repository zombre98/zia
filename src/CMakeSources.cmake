set(SERVER
        server/IClient.hpp
        server/IServer.hpp
        server/asio/AsioClient.hpp
        server/asio/AsioServer.hpp
        server/buffer.hpp
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
        ${MAIN}
        )
