//
// Created by Thomas Burgaud on 2019-02-23.
//

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
		std::cout << root << std::endl;
		std::ifstream fStream(root + std::get<dems::header::Request>(ctx.request.firstLine).path);
		std::string s((std::istreambuf_iterator<char>(fStream)),
									std::istreambuf_iterator<char>());
		ctx.response.body = s;
		ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
		ctx.response.headers->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
		std::cout << "Leaving LoadPath Module without problem" << std::endl;
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
