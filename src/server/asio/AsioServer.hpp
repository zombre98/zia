#pragma once

#include <asio.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#include "Utils/Logger.hpp"
#include "server/IServer.hpp"
#include "AsioClient.hpp"
#include "server/ModulesManager.hpp"
#include "Utils/ThreadPool.hpp"
#include "Utils/FileWatcher.hpp"
#include "Utils/JsonParser.hpp"

namespace zia {

constexpr char MODULES_PATH[] = "/var/modules/";
constexpr char CONFIG_DIRECTORY[] = "/var/config/";

class AsioServer : public IServer {
public:
  /**
	 * Construct Asio Server with an ip and a port
	 * @param ip the ip as a string
	 * @param port the port
	 */
  AsioServer(const std::string &ip, unsigned short port) :
    acceptor_(service_, asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port)) {
    thp_.addTask<void>([this]() {
      using namespace std::chrono_literals;
			try {
				zia::utils::FileWatcher fw(CONFIG_DIRECTORY, 5000ms);

				fw.watch([this](const std::string &file, zia::utils::FileWatcher::State status) {
					std::cout << file << std::endl;
					if (status == zia::utils::FileWatcher::State::MODIFIED) {
						std::unique_lock<std::mutex> clientLock(clientMutex_);
						zia::utils::JsonParser jsp(file);

						for (auto &client : clients_)
							dems::header::constructConfig(client->getContext(), jsp);
					}
				});
			} catch (std::exception &e) {
				std::cout << "Cannot find the directory: " <<  e.what() << std::endl;
			}
		});

    thp_.addTask<void>([this]() {
			using namespace std::chrono_literals;
			try {
				zia::utils::FileWatcher fw(MODULES_PATH, 5000ms);

				fw.watch([this](const std::string &file, zia::utils::FileWatcher::State status) {
					std::cout << file << std::endl;
					if (status == zia::utils::FileWatcher::State::CREATED) {
						logging::debug << LOG_DEBUG << "New Module Path : " << file << std::endl;
						moduleManager_.loadOneModule(file);
					}
					if (status == zia::utils::FileWatcher::State::DELETED) {
						logging::debug << LOG_DEBUG << "Unload Module Path : " << file << std::endl;
						auto &modules = moduleManager_.getModulesLoaded();

						if (modules.find(file) != modules.end())
							moduleManager_.unloadModule(modules[file]);
					}
				});
			} catch (std::exception &e) {
				std::cout << "Cannot find the directory: " <<  e.what() << std::endl;
			}
    });
    try {
			moduleManager_.loadModules(MODULES_PATH);
		} catch (std::exception &e) {
    	std::cout << "Cannot find the directory: " <<  e.what() << std::endl;
    }
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
      std::unique_lock<std::mutex> clientLock(clientMutex_);
      zia::utils::JsonParser jsp(std::string(CONFIG_DIRECTORY) + "config.json");

      clients_.push_back(std::move(newClient_));
      clients_.back()->getContext().request.headers = std::make_unique<dems::header::Heading>();
      clients_.back()->getContext().response.headers = std::make_unique<dems::header::Heading>();
      dems::header::constructConfig(clients_.back()->getContext(), jsp);
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

  zia::ThreadPool thp_;
  std::mutex clientMutex_;
};

}
