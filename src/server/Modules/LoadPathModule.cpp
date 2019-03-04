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
	manager.request().hookToMiddle(2, MODULE_NAME, [](dems::Context &ctx) {
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto path = root + std::get<dems::header::Request>(ctx.request.firstLine).path;
		if (std::filesystem::is_directory(path)) {
		  auto &defaultFile = std::get<std::string>(ctx.config["default_file"].v);
		  path = root + defaultFile;
		}
    std::ifstream fStream(path);
		std::string s((std::istreambuf_iterator<char>(fStream)), std::istreambuf_iterator<char>());
		ctx.response.body = std::move(s);
		return dems::CodeStatus::OK;
	});

	manager.request().hookToEnd(1, MODULE_NAME, [](dems::Context &ctx) {
		ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
		ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
