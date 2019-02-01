#pragma once

#include <string>
#include <functional>
#include "IClient.hpp"

namespace nkpp {

class IServer {
protected:
  using onConnected = std::function<void(IClient &)>;
public:
  virtual void run() = 0;
  virtual void bind(const std::string &ip, unsigned short port) = 0;
  virtual void whenOnConnected(onConnected &&callback) = 0;
  virtual void whenOnDisconnected(IClient::onDisconnected &&callback) = 0;
  virtual void stop() = 0;
  virtual ~IServer() = default;
};

}
