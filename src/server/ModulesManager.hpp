#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include "server/api/AModulesManager.hpp"
#include "DlWrapper.hpp"

namespace dems {

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
			auto fnc = handler.getSymbol<void(*)(StageManager &)>("registerHooks");

			std::cout << "Le Path " << filePath << std::endl;
			fnc(getStageManager());
			const auto &end = getStageManager().request().endsHooks();
			auto it = std::find_if(end.begin(), end.end(), [](auto const &pair) { return pair.first == "testmodule"; });
			if (it != end.end()) {
//				dems::headers::Heading heading;
				std::cout << "Module well added" << std::endl;
	//			dems::Context lol{{dems::headers::Request{"HTTP/1.1", "200", ""}, std::make_unique<dems::headers::Heading>(), "desbarres"},
			//								{dems::headers::Response{"HTTP/1.1", "200", ""},std::make_unique<dems::headers::Heading>(), "desbarres"}, 0};
		//		std::cout << std::get<dems::headers::Request>(lol.response.variant).path << std::endl;
				//it->second(lol);
			}
		} catch (const std::exception &e) {
			std::cout << "Dl Error: " << e.what() << std::endl;
		}
	}

};

}

/*
 * TODO:
 *  - Doc
 *  - Big Readme
 *  - testing
 */
