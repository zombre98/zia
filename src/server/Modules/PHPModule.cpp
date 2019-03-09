//
// Created by Thomas Burgaud on 2019-02-23.
//

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "LoadPathModule.hpp"
#include "../api/AModulesManager.hpp"
#include "../../Utils/Utils.hpp"
#include "../api/Config.hpp"

static constexpr char MODULE_NAME[] = "LoadPath";

extern "C" {

/**
 * The fonction requested by the ModuleManager to load the module
 * @param manager The Stage manager to hook the module
 */
std::string registerHooks(dems::StageManager &manager) {
	manager.request().hookToMiddle(2, MODULE_NAME, [](dems::Context &ctx) {
		auto &requestFirstLine = std::get<dems::header::Request>(ctx.request.firstLine);
		auto &root = std::get<std::string>(ctx.config["root"].v);
		auto requestPath = std::get<dems::header::Request>(ctx.request.firstLine).path;

		std::string path = root + requestPath;
		if (requestPath.find('?') != std::string::npos) {
			auto request = zia::utils::split(requestPath, '?');
			if (request.size() > 1)
				path = root + request[0];
		}

		if (std::filesystem::path(path).extension() != ".php")
			return dems::CodeStatus::DECLINED;

		if (requestFirstLine.method == "GET") {
			auto request = zia::utils::split(requestPath, '?');

			setenv("GATEWAY_INTERFACE", "CGI/1.1", 0);
			setenv("REQUEST_METHOD", "GET", 1);
			setenv("REQUEST_SCHEME", "http", 0);
			setenv("SCRIPT_FILENAME", path.c_str(), 1);
			if (request.size() > 1)
				setenv("QUERY_STRING", request[1].c_str(), 1);

		}

		auto command = "php-cgi " + path;
		FILE *file = popen(command.c_str(), "r");

		int ch;
		std::string tmp;
		while((ch = fgetc(file)) != EOF) {
			tmp.push_back(ch);
		}
		pclose(file);

		std::istringstream dataStream(tmp);
		std::string line{""};
		while (std::getline(dataStream, line)) {
			if (line == "\r")
				break;
			std::cout << "Header line " << line << std::endl;

			auto members = zia::utils::split(line, ' ');
			if (!members[0].empty() && !members[1].empty()) {
				members[0].resize(members[0].size() - 1);
				if (!members[0].empty() && !members[1].empty()) {
					members[1].erase(members[1].size() - 1, 1);
					ctx.response.headers->setHeader(members[0], members[1]);
				}
			}
		}
		while (std::getline(dataStream, line)) {
			std::cout << "With getline : " << line << std::endl;
			ctx.response.body.append(line);
		}
		std::cout << "Before leaving the php module" << std::endl << ctx.response.body << std::endl;
		return dems::CodeStatus::OK;
	});
	return MODULE_NAME;
}

};
