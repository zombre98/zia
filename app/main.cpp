#include <iostream>
#include <server/ModulesManager.hpp>

#define ASIO_STANDALONE 1
#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"

int main() {
  nkpp::AsioServer serv("0.0.0.0", 4242);
  dems::ModulesManager modManager;

  modManager.loadModules("modules/");
  serv.whenOnConnected([](nkpp::IClient &client) {
    client.whenOnRead([&client](nkpp::Buffer &b){
      logging::debug << LOG_TIME << b.read<std::string>() << std::endl;
/*      nkpp::Buffer sendBuffer;
      sendBuffer.write<std::string>("HTTP/1.1 200\r\n");
      sendBuffer.write<std::string>("Content-Length: 28\r\n");
      sendBuffer.write<std::string>("Host: 0.0.0.0:4242\r\n\r\n");
      sendBuffer.write<std::string>("<html><h1>Coucou</h1></html>\r\n\r\n");
      client.write(sendBuffer);*/
    });
  });
  serv.whenOnDisconnected([](nkpp::IClient &client){
  });
  serv.run();
  return 0;
}
