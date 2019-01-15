#pragma once

#include "../IServer.hpp"
#include <asio.hpp>
#include "AsioClient.hpp"

namespace nkpp {

class AsioServer : public IServer {
public:
  AsioServer(const std::string &ip, unsigned short port) :
  acceptor_(service_, asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port)) {
    startAccept();
  }

  void run() override {
    service_.run();
  }

  void bind(const std::string &ip, unsigned short port) override {
    acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port));
  }

  void whenOnConnected(onConnected &&callback) override {
    connectedCallback_ = std::move(callback);
  }

  void whenOnDisconnected(IClient::onDisconnected &&callback) override {
    disconnectedCallback_ = std::move(callback);
  }

  void stop() override {
    service_.stop();
  }

  ~AsioServer() override = default;

private:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
  void startAccept() {
    AsioClient::AsioClientUPtr newClient = std::make_unique<AsioClient>(service_, [this](IClient &client){
      if (disconnectedCallback_)
        disconnectedCallback_(client);
    });

    acceptor_.async_accept(newClient->socket(), [nc = std::move(newClient), this](asio::error_code) mutable {
      std::cout << "New Connection !" << std::endl;
      clients_.push_back(std::move(nc));
      if (connectedCallback_)
        connectedCallback_(*clients_.back());
      clients_.back()->read();
      startAccept();
    });
  }
#pragma clang diagnostic pop

private:
  onConnected connectedCallback_;
  IClient::onDisconnected disconnectedCallback_;

  asio::io_service service_;
  asio::ip::tcp::acceptor acceptor_;

  std::vector<AsioClient::AsioClientUPtr> clients_;

};

}
