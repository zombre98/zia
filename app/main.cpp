#include <iostream>
#include <server/ModulesManager.hpp>

#define ASIO_STANDALONE 1
#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"
#include "header/ResponseHeading.hpp"

int main() {

  /*zia::AsioServer serv("0.0.0.0", 4242);

  serv.whenOnConnected([](zia::IClient &client) {
    client.whenOnRead([&client](zia::Buffer &b){
      logging::debug << LOG_TIME << b.read<std::string>() << std::endl;
			header::ResponseHeading responseHeading(http::StatusCode::OK);
			responseHeading.setHeader("Content-Length", "28");
			responseHeading.setHeader("Host", "0.0.0.0:4242");
			responseHeading.setHeader("NewValue", "Coucou");
			zia::Buffer sendBuffer;
			sendBuffer.write<std::string>(responseHeading);
      sendBuffer.write<std::string>("<html><h1>Coucou</h1></html>\r\n\r\n");
      client.write(sendBuffer);
    });
  });
  serv.whenOnDisconnected([](zia::IClient &client){
  });
  serv.run();*/
  return 0;
}
