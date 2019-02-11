#pragma once

#include <asio.hpp>
#include "Utils/Logger.hpp"
#include "server/IServer.hpp"
#include "AsioClient.hpp"

namespace nkpp {

class AsioServer : public IServer {
public:
	/**
	 * Construct Asio Server with an ip and a port
	 * @param ip the ip as a string
	 * @param port the port
	 */
  AsioServer(const std::string &ip, unsigned short port) :
  acceptor_(service_, asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port)) {
    startAccept();
  }

  /**
	 * Running the server
	 */
  void run() override {
    service_.run();
  }

  /**
   * Binding the server to an ip and a port
   * @param ip Ip as a string
   * @param port The port
   */
  void bind(const std::string &ip, unsigned short port) override {
    acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port));
  }

  /**
   * Return the ip adress as a string
   * @return
   */
  std::string getIpAddress() {
      return acceptor_.local_endpoint().address().to_string();
  }

  /**
	 * Set a callback when a client connect to the server
	 * @param callback The callback to call
	 */
  void whenOnConnected(onConnected &&callback) override {
    connectedCallback_ = std::move(callback);
  }

  /**
	 * Set a callback to call on disconnection
	 * @param callback The callback to call
	 */
  void whenOnDisconnected(IClient::onDisconnected &&callback) override {
    disconnectedCallback_ = std::move(callback);
  }

  /**
   * Stop the server
   */
  void stop() override {
    service_.stop();
  }

  /**
   * The destructor
   */
  ~AsioServer() override = default;

private:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
  /**
   * Start accepting connections
   */
  void startAccept() {
    newClient_ = std::make_unique<AsioClient>(service_, [this](IClient &client){
      if (disconnectedCallback_)
        disconnectedCallback_(client);
    });

    acceptor_.async_accept(newClient_->socket(), [this](asio::error_code) mutable {
      clients_.push_back(std::move(newClient_));
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
  AsioClient::AsioClientUPtr newClient_;
};

}
