#pragma once

#include <string>
#include <functional>
#include "IClient.hpp"

/** @file
 * @brief Class @ref IServer
 */
/** @namespace zia
 * @brief zia project
 */
namespace zia {

/**
 * @brief A Server interface
 */
class IServer {
protected:
  using onConnected = std::function<void(IClient &)>;
public:
	/**
	 * Running the server
	 */
  virtual void run() = 0;
  /**
   * Binding the server to an ip and a port
   * @param ip Ip as a string
   * @param port The port
   */
  virtual void bind(const std::string &ip, unsigned short port) = 0;
	/**
	 * Set a callback when a client connect to the server
	 * @param callback The callback to call
	 */
	virtual void whenOnConnected(onConnected &&callback) = 0;
	/**
	 * Set a callback to call on disconnection
	 * @param callback The callback to call
	 */
  virtual void whenOnDisconnected(IClient::onDisconnected &&callback) = 0;
  /**
   * Stop the server
   */
  virtual void stop() = 0;
  /**
   * The destructor
   */
  virtual ~IServer() = default;
};

}
