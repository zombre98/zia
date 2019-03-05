//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
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

		boost::asio::ssl::context sslContext(boost::asio::ssl::context::sslv23);

		sslContext.set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use);
		sslContext.set_password_callback([&ssl_pass](std::size_t, boost::asio::ssl::context::password_purpose) { return ssl_pass;});
		sslContext.use_certificate_chain_file(cert_path);
		sslContext.use_private_key_file(cert_path, boost::asio::ssl::context::pem);
		//sslContext.use_certificate_chain_file("server.pem");
		//sslContext.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
		//sslContext.use_tmp_dh_file("dh512.pem");
		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
