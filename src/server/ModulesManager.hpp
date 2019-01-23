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
	void loadModules(const std::string &directoryPath) override {
		for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
			if (entry.is_regular_file()) {
				loadOneModule(entry.path());
			}
		}
	}

	void loadOneModule(const std::string &filePath) override {
		zia::DlWrapper handler;

		try {
			handler.open(filePath);
			auto fnc = handler.getSymbol<void(*)(ModulesManager &)>("registersHook");

			fnc(*this);
		} catch (const std::exception &e) {
			std::cout << "Dl Error: " << e.what() << std::endl;
		}
	}

};

}