#include <iostream>

#define ASIO_STANDALONE 1
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"


int main() {
  nkpp::AsioServer serv("0.0.0.0", 4242);

  serv.whenOnConnected([](nkpp::IClient &client) {
    client.whenOnRead([](nkpp::Buffer &){
      std::cout << "Reading !" << std::endl;
    });
  });
  serv.whenOnDisconnected([](nkpp::IClient &){
    std::cout << "He is disconnected !" << std::endl;
  });
  serv.run();
  return 0;
}
