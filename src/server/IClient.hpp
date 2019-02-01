#pragma once

#include "buffer.hpp"

namespace nkpp {

class IClient {
protected:
  using onRead = std::function<void(Buffer &buffer)>;
public:
  using onDisconnected = std::function<void(IClient &)>;
public:
  virtual void write(const Buffer &buffer) = 0;
  virtual void whenOnRead(onRead &&callback) = 0;
  virtual void whenOnDisconnected(onDisconnected &&callback) = 0;
  virtual ~IClient() = default;
};

}
