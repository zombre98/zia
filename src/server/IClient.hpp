#pragma once

#include <server/header/ResponseHeading.hpp>
#include "buffer.hpp"

/** @file
 * @brief Class @ref IClient
 */
namespace zia {

/**
 * @brief A Client interface
 */
class IClient {
protected:
  using onRead = std::function<void(Buffer &buffer)>;
public:
  using onDisconnected = std::function<void(IClient &)>;
public:
	/**
	 * send data to the client
	 * @param buffer The data to send
	 */
  virtual void write(const Buffer &buffer) = 0;
  /**
   * Set a callback to call on read
   * @param callback The callback to call
   */
  virtual void whenOnRead(onRead &&callback) = 0;
  /**
   * Set a callback to call on disconnection
   * @param callback The callback to call
   */
  virtual void whenOnDisconnected(onDisconnected &&callback) = 0;
  /**
   *
   * @return The Context
   */
  virtual dems::Context &getContext() = 0;
  /**
   *
   * @return The const context
   */
  virtual const dems::Context &getContext() const = 0;
  /**
   *
   * @return The Heading as const
   */
  virtual const dems::header::Heading &getHeading() const = 0;
  /**
   *
   * @return The Heading
   */
  virtual dems::header::Heading &getHeading() = 0;
  /**
   *
   * @return The raw socket
   */
  virtual int getRawSocket() = 0;
  /**
   * The desctructor
   */
  virtual ~IClient() = default;
};

}
