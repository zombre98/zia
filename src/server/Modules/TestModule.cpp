#include <iostream>
#include "server/api/AModulesManager.hpp"

static constexpr char MODULE_NAME[] = "Logger";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
void registersHook(dems::StageManager &manager) {
	manager.requests().hookToFirst(MODULE_NAME, [](dems::Context &ctx) {
		std::cout << "Stage: Request FIRST" << std::endl;
		std::cout << ctx.response.body << std::endl;
		return dems::CodeStatus::OK;
	});

	manager.requests().hookToMiddle(MODULE_NAME, [](dems::Context &ctx) {
		std::cout << "Stage: Request MIDDLE" << std::endl;
		std::cout << ctx.response.body << std::endl;
		return dems::CodeStatus::OK;
	});

	manager.requests().hookToEnd(MODULE_NAME, [](dems::Context &ctx) {
		std::cout << "Stage: Request END" << std::endl;
		std::cout << ctx.response.body << std::endl;
		return dems::CodeStatus::OK;
	});
}

};
