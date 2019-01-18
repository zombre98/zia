#include <memory>

#pragma once

#include "../IClient.hpp"
#include <asio.hpp>
#include <iostream>

namespace nkpp {

class AsioClient : public IClient {
public:
  using AsioClientUPtr = std::unique_ptr<AsioClient>;

  explicit AsioClient(asio::io_service &ios, onDisconnected &&callback) : onDisconnectedCallback_(std::move(callback)),
                                                                          socket_(ios), repeatRead_(true)
  {}
public:
  void write(const Buffer &buffer) override {
  	logging::debug << LOG_DEBUG << buffer.getBufferContainer().size() << std::endl;
    asio::async_write(socket_, asio::buffer(buffer.getBufferContainer()), [](asio::error_code, std::size_t){
      logging::debug << LOG_DEBUG << "writed some byted" << std::endl;
    });
  }

  void whenOnRead(onRead &&callback) override {
    onReadCallback_ = std::move(callback);
  }

  void whenOnDisconnected(onDisconnected &&callback) override {
    onDisconnectedCallback_ = std::move(callback);
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
  void read() {
    socket_.cancel();
    logging::debug << LOG_DEBUG << "Read Some" << std::endl;
    socket_.async_read_some(asio::buffer(buffer_.getBufferContainer()), [this](asio::error_code error, std::size_t) {
      if (!onReadCall(error))
        return;
      if (repeatRead_)
        read();
    });
  }
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
  void read(const std::string &delim) {
    socket_.cancel();
    std::cout << "Reading Until" << std::endl;
    asio::async_read_until(socket_, streamBuffer_, delim, [delim, this](asio::error_code error, std::size_t) {
      std::istream is(&streamBuffer_);
      std::string streamData = getUntilDelim(is, delim);

      std::cout << "RECEIVED:" << streamData << std::endl;
      buffer_.setData(streamData.data(), streamData.size());
      if (!onReadCall(error))
        return;
      if (repeatRead_)
        read(delim);
    });
  }
#pragma clang diagnostic pop

  void setRepeatRead(bool val) { repeatRead_ = val; socket_.cancel(); }

  asio::ip::tcp::socket &socket() { return socket_; }

private:
  bool onReadCall(asio::error_code error) {
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

private:
  onRead onReadCallback_;
  onDisconnected onDisconnectedCallback_;

  asio::io_service service_;
  asio::ip::tcp::socket socket_;
  Buffer buffer_;
  asio::streambuf streamBuffer_;

  std::atomic_bool repeatRead_;
};

}
