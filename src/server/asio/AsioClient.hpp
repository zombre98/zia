#include <memory>

#pragma once

#include <memory>
#include <asio.hpp>
#include "Utils/ThreadPool.hpp"
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include "server/header/ResponseHeading.hpp"
#include "../IClient.hpp"
namespace zia {

class AsioClient : public IClient {
public:
  using AsioClientUPtr = std::unique_ptr<AsioClient>;
  using beforeReadFnc = std::function<void(IClient &client)>;

  explicit AsioClient(asio::io_service &ios, onDisconnected &&callback) : onDisconnectedCallback_(std::move(callback)),
                                                                          socket_(ios), repeatRead_(true)
  {
    context_.response.headers = std::make_unique<dems::header::Heading>();
  }

  ~AsioClient() {
    setRepeatRead(false);
  }
public:
  /**
	 * send data to the client
	 * @param buffer The data to send
	 */
  void write(const Buffer &buffer) override {
    asio::async_write(socket_, asio::buffer(buffer.getBufferContainer(), buffer.getWroteSize()), [](asio::error_code, std::size_t) {
    });
  }

  /**
   * Set a callback to call on read
   * @param callback The callback to call
   */
  void whenOnRead(onRead &&callback) override {
    onReadCallback_ = std::move(callback);
  }

  /**
   * Set a callback to call on disconnection
   * @param callback The callback to call
   */
  void whenOnDisconnected(onDisconnected &&callback) override {
    onDisconnectedCallback_ = std::move(callback);
  }

  void setBeforeRead(beforeReadFnc &&callback) {
    beforeReadCallback_= std::move(callback);
  }

  int getRawSocket() override {
    return socket_.native_handle();
  }

  /**
   * Read from the client without delimitation
   */
  void read() {
  	threadPool_.addTask<void>([this]() {
      asio::error_code ec;
      socket_.wait(asio::ip::tcp::socket::wait_read);
      if (beforeReadCallback_)
        beforeReadCallback_(*this);
      std::size_t bTransfered = socket_.read_some(asio::buffer(buffer_.getBufferContainer()), ec);

      if (!onReadCall(ec, bTransfered))
        return;
      if (repeatRead_)
        read();
  	});
  }

  /**
   * Read from the client until a delimitation
   * @param delim The delimitation
   */
  void read(const std::string &delim) {
    threadPool_.addTask<void>([this, delim]() {
      asio::error_code ec;

      socket_.cancel();
      if (beforeReadCallback_)
        beforeReadCallback_(*this);
      std::size_t bTransfered = asio::read_until(socket_, streamBuffer_, delim);
      std::istream is(&streamBuffer_);
      std::string streamData = getUntilDelim(is, delim);

      buffer_.setData(streamData.data(), streamData.size());
      if (!onReadCall(ec, bTransfered))
        return;
      if (repeatRead_)
        read(delim);
    });
  }

  /**
   * Read data continuously or not
   */
  void setRepeatRead(bool val) { repeatRead_ = val; socket_.cancel(); }

  /**
   * Get the asio socket
   * @return
   */
  asio::ip::tcp::socket &socket() { return socket_; }

private:
  /**
	 * Function called on read, return false on error
	 * @param error If an error happend
	 * @param bytesTransfered The byted transfered
	 * @return
	 */
  bool onReadCall(asio::error_code error, std::size_t bytesTransfered) {
    buffer_.setCursor(bytesTransfered);
    if (error) {
      if (onDisconnectedCallback_)
        onDisconnectedCallback_(*this);
      buffer_.clear();
      return false;
    }
    if (onReadCallback_)
      onReadCallback_(buffer_);
    buffer_.clear();
    return true;
  }

  /**
   * Return the string cut until the delimiter
   * @param is The stream to get the string
   * @param delim the delimiter
   * @return
   */
  static std::string getUntilDelim(std::istream &is, const std::string &delim) {
    std::string str;
    char buf;

    while (is) {
      is.read(&buf, sizeof(char));
      str.push_back(buf);
      if (str.find(delim) != std::string::npos)
        return str;
    }
    return str;
  }

public:

  dems::Context &getContext() override {
    return context_;
  }

  const dems::Context &getContext() const override {
    return context_;
  }

private:
  onRead onReadCallback_;
  onDisconnected onDisconnectedCallback_;
  beforeReadFnc beforeReadCallback_;

  asio::io_service service_;
  asio::ip::tcp::socket socket_;
  Buffer buffer_;
  asio::streambuf streamBuffer_;

  std::atomic_bool repeatRead_;

  dems::Context context_;
  dems::header::Heading heading_;
  zia::ThreadPool threadPool_;

};

}
