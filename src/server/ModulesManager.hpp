#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>

#include "Utils/Logger.hpp"
#include "server/api/AModulesManager.hpp"
#include "DlWrapper.hpp"

namespace zia {

class ModulesManager : public dems::AModulesManager {
	using moduleCallback = std::function<void(dems::Context &)>;
public:
	ModulesManager() = default;
	~ModulesManager() override {
		unloadModules();
	}

public:
	/**
	 * Load all the modules present in a directory
	 * @param directoryPath the path to the directory containing the modules
	 */
	void loadModules(const std::string &directoryPath) override {
		for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
			if (entry.is_regular_file()) {
				loadOneModule(entry.path().string( ));
			}
		}
	}

	/**
	 * Load one module
	 * @param filePath the path to the module
	 */
	void loadOneModule(const std::string &filePath) override {
		handlers_[filePath] = zia::DlWrapper();
		try {
			handlers_[filePath].open(filePath);
			auto fnc = handlers_[filePath].getSymbol<std::string(*)(dems::StageManager &)>("registerHooks");

			logging::debug << LOG_DEBUG << "Module Path : " << filePath << std::endl;
			modulesNames_[filePath] = fnc(getStageManager());
		} catch (const std::exception &e) {
			logging::error << "Dl Error: " << e.what() << std::endl;
		}
	}

	/**
	* Unload a Module
	* @param moduleName The module to unload
	*/
	void unloadModule(const std::string &moduleName) override {
		getStageManager().connection().unhookAll(moduleName);
		getStageManager().request().unhookAll(moduleName);
		getStageManager().chunks().unhookAll(moduleName);
		getStageManager().disconnect().unhookAll(moduleName);
		modulesNames_.erase(moduleName);
	}

	void unloadModules() {
		for (const auto &[path, name] : modulesNames_) {
			unloadModule(name);
		}
	}

	auto &getModulesLoaded() {
		return modulesNames_;
	}

private:
	// Module Path - Module Name
	std::unordered_map<std::string, std::string> modulesNames_;
	std::unordered_map<std::string, zia::DlWrapper> handlers_;
};

}
