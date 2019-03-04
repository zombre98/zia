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

set(HEADER
        server/header/HeaderEnum.hpp
        server/header/ResponseHeading.hpp)

set(UTILS
        Utils/Logger.hpp
        Utils/JsonParser.hpp
        Utils/FileWatcher.hpp
        Utils/ThreadPool.hpp
        Utils/Utils.hpp
        )

set(MAIN
        ../app/main.cpp
        )

set(SRCS
        ${SERVER}
        ${HEADER}
        ${UTILS}
        ${API}
        ${MAIN}
        )
