//
// Created by Thomas Burgaud on 2019-02-23.
//

#include "LoadPathModule.hpp"
#include "../api/AModulesManager.hpp"
#include "../../Utils/Logger.hpp"

static constexpr char MODULE_NAME[] = "LoadPath";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
std::string registerHooks(dems::StageManager &manager) {
	manager.request().hookToFirst(2, MODULE_NAME, [](dems::Context &ctx) {
		std::cout << "I'm in HookFirst of LoadPathModule" << std::endl;
		ctx.request.firstLine = dems::header::Request{"GET", "./test.html", "HTTP/1.1"};
		std::cout << std::get<dems::header::Request>(ctx.request.firstLine).path << std::endl;
		std::ifstream fStream(std::get<dems::header::Request>(ctx.request.firstLine).path);
		std::string s((std::istreambuf_iterator<char>(fStream)),
									std::istreambuf_iterator<char>());
		ctx.response.body = s;
		std::cout << ctx.response.body << std::endl;
		ctx.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
		ctx.response.headersheaders->setHeader("Content-Length", std::to_string(ctx.response.body.length()));
		return dems::CodeStatus::OK;
	});

	return MODULE_NAME;
}

};
