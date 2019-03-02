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

		SSL_library_init();
		SSL_METHOD const *meth = SSLv23_server_method();
		if (!(ssl_ctx = SSL_CTX_new(meth)))
			throw std::runtime_error("Error while init ssl");
		if (SSL_CTX_set_cipher_list(ssl_ctx, "AES128-SHA") <= 0)
			throw std::runtime_error("Error while set cipher list");
		if (SSL_CTX_use_certificate_file(ssl_ctx, cert_path.c_str(), SSL_FILETYPE_PEM) <= 0)
			throw std::runtime_error("Error while using the certificate");
		SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, (void *)ssl_pass.c_str());
		if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key_path.c_str(), SSL_FILETYPE_PEM) <= 0)
			throw std::runtime_error("Error while using the private key");
		if (SSL_CTX_check_private_key(ssl_ctx) == 0)
			throw std::runtime_error("Certificat and key doesn't match");
		SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
		if (SSL_CTX_load_verify_locations(ssl_ctx, cert_path.c_str(), NULL) < 1)
			throw std::runtime_error("Setting the verify location");
		//	if (SSL_CTX_load_and_set_client_CA_file(ssl_ctx, cert_path.c_str()) < 1)
		// throw std::runtime_error("Error setting CA file");

		myssl = SSL_new(ssl_ctx);
		if (!myssl)
			throw std::runtime_error("SSL_new failed");
		SSL_set_fd(myssl, ctx.socketFd);
		if (SSL_accept(myssl) < 1)
			throw std::runtime_error("SSL accept failed");
		if (SSL_get_verify_result(myssl) != X509_V_OK)
			throw std::runtime_error("SSL Client Authentication error");
		std::cout << "SSL connection on socket " << ctx.socketFd << ", Version: " << SSL_get_version(myssl) << ", Cipher: " << SSL_get_cipher(myssl) << std::endl;
		char buff[1024];
		if (SSL_read(myssl, buff, sizeof(buff)) < 1)
			throw std::runtime_error("SSL read failed");
		std::cout << "Receive from client :" << std::endl << buff << std::endl;

		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
