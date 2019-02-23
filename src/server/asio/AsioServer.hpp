#pragma once

#include <asio.hpp>
#include "Utils/Logger.hpp"
#include "server/IServer.hpp"
#include "AsioClient.hpp"
#include "server/ModulesManager.hpp"

namespace zia {

constexpr char MODULES_PATH[] = "/tmp/modules";

class AsioServer : public IServer {
public:
	/**
	 * Construct Asio Server with an ip and a port
	 * @param ip the ip as a string
	 * @param port the port
	 */
  AsioServer(const std::string &ip, unsigned short port) :
  acceptor_(service_, asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port)) {
    moduleManager_.loadModules(MODULES_PATH);
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

  zia::ModulesManager &getModulesManager() {
    return moduleManager_;
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
    disconnectedCallback_ = [this, callIner = std::move(callback)](IClient &client) {
      for (auto &first : moduleManager_.getStageManager().disconnect().firstHooks()) {
        first.second.callback(client.getContext());
      }
      for (auto &middle : moduleManager_.getStageManager().disconnect().middleHooks()) {
        middle.second.callback(client.getContext());
      }
      for (auto &last : moduleManager_.getStageManager().disconnect().endHooks()) {
        last.second.callback(client.getContext());
      }
      callIner(client);
    };
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
  /**
   * Start accepting connections
   */
  void startAccept() {
    newClient_ = std::make_unique<AsioClient>(service_, [this](IClient &client){
      if (disconnectedCallback_)
        disconnectedCallback_(client);
    });

    acceptor_.async_accept(newClient_->socket(), [this](asio::error_code) {
      clients_.push_back(std::move(newClient_));
      // First Hooks
      for (auto &first: moduleManager_.getStageManager().connection().firstHooks()) {
        first.second.callback(clients_.back()->getContext());
      }
      if (connectedCallback_)
        connectedCallback_(*clients_.back());

      // Middle Hooks
      for (auto &middle: moduleManager_.getStageManager().connection().middleHooks()) {
        middle.second.callback(clients_.back()->getContext());
      }

      // Last Hooks
      for (auto &last: moduleManager_.getStageManager().connection().endHooks()) {
        last.second.callback(clients_.back()->getContext());
      }

      clients_.back()->read();
      startAccept();
    });
  }

private:
  onConnected connectedCallback_;
  IClient::onDisconnected disconnectedCallback_;

  asio::io_service service_;
  asio::ip::tcp::acceptor acceptor_;

  std::vector<AsioClient::AsioClientUPtr> clients_;
  zia::ModulesManager moduleManager_;
  AsioClient::AsioClientUPtr newClient_;
};

}
