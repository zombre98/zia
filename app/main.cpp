#include <iostream>

#define ASIO_STANDALONE 1
#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"


int main() {
  nkpp::AsioServer serv("0.0.0.0", 4242);

  serv.whenOnConnected([](nkpp::IClient &client) {
    client.whenOnRead([](nkpp::Buffer &){
      logging::debug << LOG_TIME << "Reading !" << std::endl;
    });
  });
  serv.whenOnDisconnected([](nkpp::IClient &){
    logging::debug << LOG_TIME << "He is disconnected !" << std::endl;
  });
  serv.run();
  return 0;
}
