#include <iostream>
#include "../AModulesManager.hpp"

extern "C" {

	/**
	 * The fonction requested by the ModuleManager to load the module
	 * @param manager The Stage manager to hook the module
	 */
void registersHook(api::StageManager &manager) {
	std::cout << "Je Suis Un Module Lol" << std::endl;
	manager.requests().hookToEnd("testmodule", [](api::Context &ctx) {
		std::cout << "I'm The test module lol" << std::endl;
	});
}

};
