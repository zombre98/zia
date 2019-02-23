#include <iostream>
#include <server/ModulesManager.hpp>

#define ASIO_STANDALONE 1
#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"
#include "server/header/ResponseHeading.hpp"
#include "Utils/JsonParser.hpp"

int main() {

  nkpp::AsioServer serv("127.0.0.1", 4242);
  dems::ModulesManager modManager;

  modManager.loadModules("modules/");
  serv.whenOnConnected([](nkpp::IClient &client) {
    client.whenOnRead([&client](nkpp::Buffer &b){
      logging::debug << LOG_TIME << b.read<std::string>() << std::endl;
/*
			header::ResponseHeading responseHeading(http::StatusCode::OK);
			responseHeading.setHeader("Content-Length", "28");
			responseHeading.setHeader("Host", "0.0.0.0:4242");
			responseHeading.setHeader("NewValue", "Coucou");
			nkpp::Buffer sendBuffer;
			sendBuffer.write<std::string>(responseHeading);
      sendBuffer.write<std::string>("<html><h1>Coucou</h1></html>\r\n\r\n");
      client.write(sendBuffer);
*/
    });
  });
  serv.whenOnDisconnected([](nkpp::IClient &client){
  });
  serv.run();
  return 0;
}
