//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <stdio.h>
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
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto path = root + std::get<dems::header::Request>(ctx.request.firstLine).path;

		if (std::filesystem::path(path).extension() != ".php")
			return dems::CodeStatus::DECLINED;

		auto command = "php " + path;
		std::cout << "Command :'" << command << "'" << std::endl;
		FILE *file = popen(command.c_str(), "r");

		int ch;
		std::cout << "Before push_back : " << ctx.response.body << std::endl;
		std::cout << "Start to stock" << std::endl;
		while((ch = fgetc(file)) != EOF) {
			std::cout << (char)ch;
			ctx.response.body.push_back(ch);
		}
		pclose(file);
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
