//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <sys/socket.h>
#include "../api/AModulesManager.hpp"
#include "../../Utils/Logger.hpp"
#include "../api/Config.hpp"

static constexpr char MODULE_NAME[] = "OpenSsl";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
std::string registerHooks(dems::StageManager &manager) {
	manager.request().hookToFirst(1, MODULE_NAME, [](dems::Context &ctx) {
		auto cert_path = std::get<std::string>(ctx.config["cert_path"].v);
		auto key_path = std::get<std::string>(ctx.config["key_path"].v);
		auto ssl_pass = std::get<std::string>(ctx.config["ssl_password"].v);

		std::cout << "Hi from ssl Hooktofirst request" << std::endl;
		std::cout << "From context we got : " <<  ctx.socketFd << std::endl;
		std::cout << "Key path : " << key_path << std::endl;
		std::cout << "Certificat path : " << cert_path << std::endl;
		boost::asio::io_service ioserv;
		boost::asio::ip::tcp::socket socket(ioserv);

		socket.assign(boost::asio::ip::tcp::v4(), ctx.socketFd);
		boost::asio::ssl::context sslContext(boost::asio::ssl::context::sslv23);
		sslContext.set_password_callback([&ssl_pass]() { return ssl_pass; });
		sslContext.use_certificate_chain_file(cert_path);
		sslContext.use_private_key_file(key_path, boost::asio::ssl::context::pem);
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream(ioserv, sslContext);
//		sslContext.set_verify_mode(boost::asio::ssl::verify_peer);
	//	sslContext.load_verify_file(cert_path);
		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
