//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <openssl/ssl.h>
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

		std::cout << "Key path : " << key_path << std::endl;
		std::cout << "Certificat path : " << cert_path << std::endl;

		SSL_CTX  *ssl_ctx;
		SSL  *myssl;

		int listenerSocket;

		SSL_library_init();
		SSL_METHOD const *meth = SSLv23_server_method();
		if (!(ssl_ctx = SSL_CTX_new(meth)))
			throw std::runtime_error("Error while init ssl");
		if (SSL_CTX_set_cipher_list(ssl_ctx, "AES128-SHA") <= 0)
			throw std::runtime_error("Error while set cipher list");
		if (SSL_CTX_use_certificate_file(ssl_ctx, cert_path.c_str(), SSL_FILETYPE_PEM) <= 0)
			throw std::runtime_error("Error while using the certificate");
		SSL_CTX_set_default_passwd_cb_userdata(ctx, );

		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
