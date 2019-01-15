#pragma once

#include "../IClient.hpp"
#include <asio.hpp>
#include <iostream>

namespace nkpp {

class AsioClient : public IClient {
public:
  using AsioClientUPtr = std::unique_ptr<AsioClient>;

  explicit AsioClient(asio::io_service &ios, onDisconnected &&callback) : onDisconnectedCallback_(std::move(callback)),
                                                                          socket_(ios)
  {}
public:
  void write(const Buffer &buffer) override {
    asio::async_write(socket_, asio::buffer(buffer.getBufferContainer()), [](asio::error_code, std::size_t){
      std::cout << "writed some byted" << std::endl;
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
    socket_.async_read_some(asio::buffer(buffer_.getBufferContainer()), [this](asio::error_code error, std::size_t) {
      if (error) {
        if (onDisconnectedCallback_)
          onDisconnectedCallback_(*this);
        return;
      }
      if (onReadCallback_)
        onReadCallback_(buffer_);
      buffer_.clear();
      read();
    });
  }
#pragma clang diagnostic pop

  asio::ip::tcp::socket &socket() { return socket_; }

private:
  onRead onReadCallback_;
  onDisconnected onDisconnectedCallback_;

  asio::io_service service_;
  asio::ip::tcp::socket socket_;
  Buffer buffer_;
};

}
