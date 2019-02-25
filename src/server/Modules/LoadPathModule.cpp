//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <filesystem>
#include "LoadPathModule.hpp"
#include "../api/AModulesManager.hpp"
#include "../../Utils/Logger.hpp"
#include "../api/Config.hpp"

static constexpr char MODULE_NAME[] = "LoadPath";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
std::string registerHooks(dems::StageManager &manager) {
	manager.request().hookToFirst(2, MODULE_NAME, [](dems::Context &ctx) {
		std::cout << "I'm in HookFirst of LoadPathModule" << std::endl;
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto path = root + std::get<dems::header::Request>(ctx.request.firstLine).path;
		if (std::filesystem::is_directory(path)) {
		  auto &defaultFile = std::get<std::string>(ctx.config["default_file"].v);
		  std::ifstream errorStream(root + defaultFile);
		  std::string s((std::istreambuf_iterator<char>(errorStream)), std::istreambuf_iterator<char>());
		  ctx.response.body = s;
		  ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
		  ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
		  return dems::CodeStatus::OK;
		}
		std::ifstream fStream(path);
		std::cout << "Read is open : " << fStream.is_open() << std::endl;
		std::string s((std::istreambuf_iterator<char>(fStream)), std::istreambuf_iterator<char>());
		ctx.response.body = s;
		std::cout << "In load File Module : " << s << std::endl;
		ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
		ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
		std::cout << "Leaving LoadPath Module without problem" << std::endl;
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
