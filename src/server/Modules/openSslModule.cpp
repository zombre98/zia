//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <openssl/ssl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../api/AModulesManager.hpp"
#include "../../Utils/Logger.hpp"
#include "../api/Config.hpp"

static constexpr char MODULE_NAME[] = "OpenSsl";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
static SSL_CTX  *ssl_ctx;

std::string registerHooks(dems::StageManager &manager) {
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	SSL_METHOD const *meth = SSLv23_server_method();
	if (!(ssl_ctx = SSL_CTX_new(meth)))
		throw std::runtime_error("Error while init ssl");
	SSL_CTX_set_ecdh_auto(ssl_ctx, 1);
	//if (SSL_CTX_set_cipher_list(ssl_ctx, "AES128-SHA") <= 0)
	//	throw std::runtime_error("Error while set cipher list");
	if (SSL_CTX_use_certificate_file(ssl_ctx, "./cert.pem", SSL_FILETYPE_PEM) <= 0)
		throw std::runtime_error("Error while using the certificate");
//		SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, (void *)ssl_pass.c_str());
	if (SSL_CTX_use_PrivateKey_file(ssl_ctx, "./key.pem", SSL_FILETYPE_PEM) <= 0)
		throw std::runtime_error("Error while using the private key");

	manager.request().hookToFirst(1, MODULE_NAME, [](dems::Context &ctx) {
		auto cert_path = std::get<std::string>(ctx.config["cert_path"].v);
		auto key_path = std::get<std::string>(ctx.config["key_path"].v);
		auto ssl_pass = std::get<std::string>(ctx.config["ssl_password"].v);

		SSL  *myssl;
		myssl = SSL_new(ssl_ctx);
		int err;
		std::cout << "After SSL_new" << std::endl;
		if (!myssl) {
			err = SSL_get_error(myssl, err);
			std::cout << "SSL_new failed error value : #" << err << std::endl;
			throw std::runtime_error("SSL_new failed");
		}
		SSL_set_fd(myssl, ctx.socketFd);
		std::cout << "After set_fd" << std::endl;
		err = SSL_accept(myssl);
		std::cout << "Return value of SSL_accept : " << err << std::endl;
		if (err <= 0) {
			err = SSL_get_error(myssl, err);

			std::cout << "Request is'nt a ssl request with ssl error : #" << err << std::endl;
			return dems::CodeStatus::DECLINED;
		}
//			throw std::runtime_error("SSL accept failed");
		if (SSL_get_verify_result(myssl) != X509_V_OK) {
			std::cout << "SSL_get_verify_result" << std::endl;
			throw std::runtime_error("SSL Client Authentication error");
		}
		std::cout << "SSL connection on socket " << ctx.socketFd << ", Version: " << SSL_get_version(myssl) << ", Cipher: " << SSL_get_cipher(myssl) << std::endl;
		char buff[1024];
		int value_ssl_read = SSL_read(myssl, buff, sizeof(buff));
		if (value_ssl_read < 1)
			throw std::runtime_error("SSL read failed");
		std::cout << "We've read : " << value_ssl_read << std::endl;
		std::cout << "Receive from client in ssl_read :" << std::endl << buff << std::endl;

		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
