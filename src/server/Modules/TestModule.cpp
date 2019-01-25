#include <iostream>
#include "../AModulesManager.hpp"

void lol() {
	 std::cout << "lol" << std::endl;
}

extern "C" {

void registersHook(api::StageManager &manager) {
	std::cout << "Je Suis Un Module Lol" << std::endl;
	manager.requests().hookToEnd("testmodule", [](api::Context &ctx) {
		std::cout << "I'm The test module lol" << std::endl;
	});
}

};
