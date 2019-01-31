#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include "AModulesManager.hpp"
#include "DlWrapper.hpp"

namespace api {

class ModulesManager : public AModulesManager {
	using moduleCallback = std::function<void(Context &)>;
public:
	ModulesManager() = default;

public:
	/**
	 * Load all the modules present in a directory
	 * @param directoryPath the path to the directory containing the modules
	 */
	void loadModules(const std::string &directoryPath) override {
		for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
			if (entry.is_regular_file()) {
				loadOneModule(entry.path());
			}
		}
	}

	/**
	 * Load one module
	 * @param filePath the path to the module
	 */
	void loadOneModule(const std::string &filePath) override {
		zia::DlWrapper handler;

		try {
			handler.open(filePath);
			auto fnc = handler.getSymbol<void(*)(StageManager &)>("registersHook");

			std::cout << "Le Path " << filePath << std::endl;
			fnc(getStageManager());
			const api::Stage::hookList &end = getStageManager().requests().endsHooks();
			auto it = std::find(end.begin(), end.end(), [](auto const &pair) { return pair.first == "testmodule"; });
			if (it != end.end()) {
				Heading heading;
				std::cout << "Module well added" << std::endl;
				api::Context lol{{response{"HTTP/1.1", "200", ""}, std::make_unique<Heading>(), "desbarres"},
											{response{"HTTP/1.1", "200", ""},std::make_unique<Heading>(), "desbarres"}, 0};
				it->second(lol);
			}
		} catch (const std::exception &e) {
			std::cout << "Dl Error: " << e.what() << std::endl;
		}
	}

};

}
