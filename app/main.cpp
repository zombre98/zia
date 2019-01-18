#include <iostream>

#define ASIO_STANDALONE 1
#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"


int main() {
  nkpp::AsioServer serv("0.0.0.0", 4242);

  serv.whenOnConnected([](nkpp::IClient &client) {
    client.whenOnRead([](nkpp::Buffer &b){
      logging::debug << LOG_TIME << b.read<std::string>() << std::endl;
    });
    nkpp::Buffer sendBuffer;
    sendBuffer.write<std::string>("GET / HTTP/1.1 200\r\n");
    sendBuffer.write<std::string>("Host: 127.0.0.1:4242\r\n");
    /*sendBuffer.write<std::string>("Server: Apache/1.3.27\r\n");
    sendBuffer.write<std::string>("Content-Type: text/plain\r\n");
    sendBuffer.write<std::string>("Connection: close\r\n");
    sendBuffer.write<std::string>("Age: 3387\r\n");
    sendBuffer.write<std::string>("Allow: GET\r\n");*/
    sendBuffer.write<std::string>("<html><h1>Coucou</h1></html>");
    client.write(sendBuffer);
  });
  serv.whenOnDisconnected([](nkpp::IClient &client){
  });
  serv.run();
  return 0;
}
