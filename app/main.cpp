#include <iostream>
#include <server/ModulesManager.hpp>

#define ASIO_STANDALONE 1

#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"
#include "server/header/ResponseHeading.hpp"
//#include "Utils/JsonParser.hpp"

int main() {

  zia::AsioServer serv("0.0.0.0", 4242);

  serv.whenOnConnected([&serv](zia::IClient &client) {
    client.whenOnRead([&client, &serv](zia::Buffer &b){
      auto data = b.read<std::string>();
      auto &context = client.getContext();

			std::copy(data.begin(), data.end(), std::back_inserter(context.rawData));
			dems::header::fillHeading(data, context, *context.request.headers);
			context.socketFd = client.getRawSocket();
      for (auto &first : serv.getModulesManager().getStageManager().request().firstHooks()) {
        first.second.callback(client.getContext());
      }
      for (auto &middle : serv.getModulesManager().getStageManager().request().middleHooks()) {
        middle.second.callback(client.getContext());
      }
      for (auto &end : serv.getModulesManager().getStageManager().request().endHooks()) {
        end.second.callback(client.getContext());
      }
      //logging::debug << LOG_TIME << b.read<std::string>() << std::endl;
			/*header::ResponseHeading responseHeading(http::StatusCode::OK);
			responseHeading.setHeader("Content-Length", "28");
			responseHeading.setHeader("Host", "0.0.0.0:4242");
			responseHeading.setHeader("NewValue", "Coucou");
			zia::Buffer sendBuffer;
			sendBuffer.write<std::string>(responseHeading);
      sendBuffer.write<std::string>("<html><h1>Coucou</h1></html>\r\n\r\n");
      client.write(sendBuffer);*/
    });
  });
  serv.whenOnDisconnected([](zia::IClient &client){
  });
  serv.run();
  return 0;
}
