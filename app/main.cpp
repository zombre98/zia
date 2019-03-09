#include <iostream>
#include <server/ModulesManager.hpp>
#include <Utils/JsonParser.hpp>

#define ASIO_STANDALONE 1

#include "Utils/Logger.hpp"
#include "server/asio/AsioServer.hpp"
#include "server/IClient.hpp"
#include "server/header/ResponseHeading.hpp"

int main() {
  zia::AsioServer serv("0.0.0.0", 4242);

  serv.whenOnConnected([&serv](zia::IClient &client) {
    client.whenOnRead([&client, &serv](zia::Buffer &b){
			for (auto &first : serv.getModulesManager().getStageManager().request().firstHooks()) {
				first.second.callback(client.getContext());
			}
      auto data = b.read<std::string>();
      auto &context = client.getContext();
			context.socketFd = client.getRawSocket();

			std::copy(data.begin(), data.end(), std::back_inserter(context.rawData));
			dems::header::fillHeading(data, context, *context.request.headers);
			auto &request = std::get<dems::header::Request>(context.request.firstLine);
			if (request.method.empty() && request.path.empty() && request.httpVersion.empty())
				context.response.firstLine = dems::header::Response {"HTTP/1.1", "400", "Bad Request"};
			else {
				for (auto &middle : serv.getModulesManager().getStageManager().request().middleHooks()) {
					middle.second.callback(client.getContext());
				}
				for (auto &end : serv.getModulesManager().getStageManager().request().endHooks()) {
					end.second.callback(client.getContext());
				}
			}
      auto response = dems::header::constructResponse(client.getContext());
      zia::Buffer responseBuffer;
      responseBuffer.write<std::string>(response);
      client.write(responseBuffer);
    });
  });
  serv.whenOnDisconnected([](zia::IClient &client){
  });
  serv.run();
  return 0;
}
